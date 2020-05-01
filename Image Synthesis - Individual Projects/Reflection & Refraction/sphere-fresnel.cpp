// =============================================================================
// VIZA654/CSCE646 at Texas A&M UniversiT_y
// Homework 0
// Created by Anton Agana based from Ariel Chisholm's template
// 05.23.2011
//
// This file is supplied with an associated makefile. Put both files in the same
// directory, navigate to that directory from the Linux shell, and T_yPr 'make'.
// This will create a program called 'pr01' that you can run by entering
// 'homework0' as a command in the shell.
//
// If you are new to programming in Linux, there is an
// excellent introduction to makefile structure and the gcc compiler here:
//
// http://www.cs.T_xstate.edu/labs/tutorials/tut_docs/Linux_Prog_Environment.pdf
//
// =============================================================================

#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include </home/user/Desktop/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include </home/user/Desktop/stb_image_write.h>
#include </home/user/Desktop/stb_include.h>

#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char *plane_arr_f;

int width = 200, height = 200, channels1, channels2, channels3;

int ind=0;

class Vector
{
public:
	float x,y,z;
friend Vector operator-(Vector a, Vector b)
{
	Vector res;
	res.x=a.x-b.x;
	res.y=a.y-b.y;
	res.z=a.z-b.z;
	return res;
}
friend Vector operator+(Vector a, Vector b)
{
	Vector res;
	res.x=a.x+b.x;
	res.y=a.y+b.y;
	res.z=a.z+b.z;
	return res;
}
friend float operator*(Vector a, Vector b)
{
	
	return a.x*b.x+a.y*b.y+a.z*b.z;
}
friend Vector operator*(Vector a, float b)
{
	Vector res;
	res.x=a.x*b;
	res.y=a.y*b;
	res.z=a.z*b;
	return res;
}

friend ostream& operator<<(ostream& os, Vector& v)
{
	os<<"("<<v.x<<","<<v.y<<","<<v.z<<")";
	return os;
}


};

float r=50;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================
float magnitude(float a, float b, float c)
{
	return sqrt(pow(a, 2) + pow(b, 2) + pow(c, 2));
}

Vector cross_product(Vector a, Vector b)
{
	Vector res;
	res.x=a.y*b.z-b.y*a.z;
	res.y=a.z*b.x-b.z*a.x;
	res.z=a.x*b.y-b.x*a.y;
	return res;
}


float crop(float min, float max, float x) {
	x = (x - min) / (max - min);
	if (x > 1)
		x = 1;
	if (x < 0)
		x = 0;
	return -2 * pow(x, 3) + 3 * pow(x, 2);
}

class Object
{
public: 
	Vector Pc;
	float r,ior;
	Vector P0,P1,P2;
	Vector A;
	Vector color;
	char type;
	bool reflective,refractive;
	Vector n;
	virtual void normal(Vector &P)
	{
	}
	virtual float eq(Vector &P)
	{
	}
	virtual void print()
{

}
virtual bool intersect(Vector &P, Vector &dir,float &t_hit)
	{
		}
};

class Sphere : public Object
{
public:
	
	Sphere(float x,float y,float z, float radius, Vector &c,bool refl,bool refr,char t,float ir)
	{
		Pc.x=x;
		Pc.y=y;
		Pc.z=z;
		r=radius;
		color.x=c.x;
		color.y=c.y;
		color.z=c.z;
		reflective=refl;
		refractive=refr;
		type=t;
		ior=ir;

	}


	void normal(Vector &P)
	{
		n=(P-Pc)*(1/r);

	}

	float eq(Vector &P)
	{


	return (P-Pc)*(P-Pc) - pow(r,2);
	}

	void print()
	{
		cout<<"Center:"<<Pc<<endl;
	}

	bool intersect(Vector &P, Vector &dir,float &t_hit)
	{
			float b,c,disc;
			b=dir*(P-Pc);
			c=eq(P);
			disc=sqrt(pow(b,2)-c);
			if((b>0 || isnan(disc)))
			{

			}
			else
			{

				t_hit=-b-disc;
				return true;
			}
			return false;
	}

	

};

class Plane : public Object
{
public:
	
	Plane(Vector &P, Vector &normal,Vector &c,bool refl,bool refr,char t,float ir)
	{
		Pc.x=P.x;
		Pc.y=P.y;
		Pc.z=P.z;
		n=normal;
		color.x=c.x;
		color.y=c.y;
		color.z=c.z;
		reflective=refl;
		refractive=refr;
		type=t;
		ior=ir;

	}


	void normal(Vector &P)
	{
		return;

	}

	float eq(Vector &P)
	{


	return n*(P-Pc);
	}

	void print()
	{
		cout<<"Point:"<<Pc<<endl;
	}

bool intersect(Vector &P, Vector &dir,float &t_hit)
	{
		float num=(n*(Pc-P));
			float den=(n*dir);
			if(num<0 && den<0)
			{
				t_hit=num/den;

				Vector P_hit=P+(dir*t_hit);
				
				return true;
			}
			return false;
	}
	

};


class Triangle : public Object
{
public:
	
	Triangle(Vector &S1,Vector &S2, Vector &S3, Vector &c,bool refl,bool refr,char t,float ir)
	{
		P0=S1;
		P1=S2;
		P2=S3;
		type=t;
		color=c;
		reflective=refl;
		refractive=refr;
		ior=ir;
	}
	void normal(Vector &P)
	{

	Vector T_V0,T_V1,T_V2;

	T_V0=P0-P2;
	T_V1=P1-P0;
	T_V2=P2-P1;

	
	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	n=A*(1/magnitude(A.x,A.y,A.z));

	}

	void print()
	{
		cout<<"Face:"<<P0<<","<<P1<<","<<P2<<endl;
	}

bool intersect(Vector &P, Vector &dir,float &t_hit)
	{
			
			float num=(n*(P1-P));
			float den=(n*dir);
			float t;
			if(num<0 && den<0)
			{
				t_hit=num/den;

				Vector P_hit=P+(dir*t_hit);
				
				Vector T_A0,T_A1,T_A2;

				T_A0=cross_product(P_hit-P2,P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-P0,P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-P1,P0-P_hit);
				T_A2=T_A2*0.5;


				float s,t,u;
				s=(n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(n*T_A0)/magnitude(A.x,A.y,A.z);


				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{
					return true;
				}
			}
			return false;
	}
};



Object* objects[3];
int trace(Vector &P, Vector &dir,int &index,float &t_hit)
	{
	float t_min=999;
	float ind_min=999;
	float t;

	for(int k=0;k<ind;k++)
		{
		
 		if ( objects[k]->intersect(P, dir, t) && t<t_min) { 
            ind_min = k;
            t_min=t;
           
        } 
		
	}

  
	t_hit=t_min;
	index=ind_min;

	return t_hit!=999 && index!=999;
}

float max(float a,float b)
{
	if(a>b)
		return a;
	return b;
}

float min(float a,float b)
{
	if(a>b)
		return b;
	return a;
}



void fresnel(const Vector &I, const Vector &N, const float &ior, float &kr) 
{ 
    float cosi = max(-1, min(1, (I*N)));
    float etai = 1, etat = ior; 
    if (cosi > 0) { std::swap(etai, etat); } 
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi)); 
    // Total internal reflection
    if (sint >= 1) { 
        kr = 1; 
    } 
    else { 
        float cost = sqrtf(std::max(0.f, 1 - sint * sint)); 
        cosi = fabsf(cosi); 
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)); 
        kr = (Rs * Rs + Rp*Rp) / 2; 
    } 
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
} 

Vector refract(const Vector &I, const Vector &N, const float &ior) 
{ 
    float cosi = max(-1, min(1, (I*N)));
    float etai = 1, etat = ior; 
    Vector n = N; 
    Vector def;
	def.x=0;
	def.y=0;
	def.z=0;
    if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= N*-1; } 
    float eta = etai / etat; 
    float k = 1 - eta * eta * (1 - cosi * cosi); 
    return k < 0 ? def : I*eta + n*(eta * cosi - sqrtf(k)) ; 
} 

Vector reflect(const Vector &I, const Vector &N) 
{ 
    return I - N*(2 * (I*N)); 
}

Vector castRay(Vector &Pe, Vector &npe, Vector &default_col,const int &depth=0)
{
	
	Vector L;
	L.x=(60);
	L.y=(-100);
	L.z=(50);
	
	L=L*(1/magnitude(L.x,L.y,L.z));
	Vector dark,n;
	dark.x=0;
	dark.y=0;
	dark.z=0;
	
	Vector hit_col=dark;

	int index;
	float t_hit;

	if(depth>2)
		return default_col;
	// cout<<trace(Pe,npe,index,t_hit)<<endl;
	if(trace(Pe,npe,index,t_hit))
	{

		// cout<<index<<endl<<t_hit<<endl;
		Vector P_hit=Pe+(npe*t_hit);
		objects[index]->normal(P_hit);
		Vector n=objects[index]->n;
		double T = 0.5*(L*n) + 0.5;

		bool outside = npe*n < 0; 
		Vector bias = n*0.0001;

		// float cosi=npe*n,cost;
		if(objects[index]->reflective && objects[index]->refractive)
		{
			Vector refl_col=dark;
			Vector refr_col=dark;
			// cout<<"hi"<<endl;
			float kr;
			fresnel(npe,n,objects[index]->ior,kr);
			
			// cout<<outside<<endl;
			if (kr < 1) { 
                Vector n_refr = refract(npe, n, objects[index]->ior);
                n_refr=n_refr*(1/magnitude(n_refr.x,n_refr.y,n_refr.z));

                P_hit = outside ? P_hit + bias : P_hit - bias; 
                refr_col = castRay(P_hit, n_refr, default_col,depth+1); 
            } 

                Vector n_refl = reflect(npe, n);
                n_refl=n_refl*(1/magnitude(n_refl.x,n_refl.y,n_refl.z));

                P_hit = outside ? P_hit - bias : P_hit + bias; 
                refl_col = castRay(P_hit, n_refl, default_col,depth+1); 
 
                // mix the two
                hit_col = hit_col + refl_col * (kr)+ refr_col *(1-kr)  ; 

		}
		else if(objects[index]->refractive)
		{

            Vector n_refr = refract(npe, n, objects[index]->ior);
            n_refr=n_refr*(1/magnitude(n_refr.x,n_refr.y,n_refr.z));

            Vector P_hit = outside ? P_hit + bias : P_hit - bias; 
            hit_col=castRay(P_hit, n_refr, default_col,depth+1); 

		}
		else
			hit_col=objects[index]->color;

	}
	else
		hit_col=default_col;
	return hit_col;
	
}



void setPixels()
{

Vector sph1_c,sph2_c,sph3_c,tri_c1,tri_c2,tri_c3,pl_c,p2_c;
sph1_c.x=255;
sph1_c.y=255;
sph1_c.z=0;

pl_c.x=0;
pl_c.y=0;
pl_c.z=255;

p2_c.x=0;
p2_c.y=255;
p2_c.z=255;

sph2_c.x=255;
sph2_c.y=0;
sph2_c.z=255;

sph3_c.x=255;
sph3_c.y=0;
sph3_c.z=0;

tri_c1.x=255;
tri_c1.y=255;
tri_c1.z=255;

tri_c2.x=255;
tri_c2.y=255;
tri_c2.z=255;

tri_c3.x=255;
tri_c3.y=255;
tri_c3.z=255;



Vector P_p1;
P_p1.x=50;
P_p1.y=-350;
P_p1.z=0;

Vector P_p2;
P_p2.x=50;
P_p2.y=-150;
P_p2.z=-20;

Vector pn2,pn1;
pn1.x=1;
pn1.y=1;
pn1.z=0;
pn1=pn1*(1/magnitude(pn1.x,pn1.y,pn1.z));

pn2.x=-1;
pn2.y=1;
pn2.z=3;
pn2=pn2*(1/magnitude(pn2.x,pn2.y,pn2.z));

Sphere *sp=new Sphere(-40,20,-20,40,sph2_c,true,true,'s',1.66);
objects[ind]= sp;
ind+=1;
sp=new Sphere(30,-30,80,50,sph1_c,false,false,'s',1);
objects[ind]= sp;
ind+=1;
Plane *p=new Plane(P_p2,pn2,p2_c,false,false,'p',1);
objects[ind]= p;
ind+=1;

p=new Plane(P_p1,pn1,sph3_c,false,false,'p',1);
objects[ind]= p;
ind+=1;
// sp=new Sphere(-100,140,50,20,sph2_c,false,false,'s',1);
// objects[ind]= sp;
// ind+=1;


Vector default_col;
default_col.x=255;
default_col.y=255;
default_col.z=255;

Vector Vup;
Vup.x=1;
Vup.y=1;
Vup.z=1;

Vector V_view;
V_view.x=0;
V_view.y=-1;
V_view.z=0;

Vector V0= cross_product(V_view,Vup);
Vector n0;
n0=V0*(1/magnitude(V0.x,V0.y,V0.z));
Vector n2;
n2=V_view*(1/magnitude(V_view.x,V_view.y,V_view.z));

Vector n1= cross_product(n0,n2);

Vector Pe;
Pe.x=0;
Pe.y=125;
Pe.z=3;

float d=100,sx=200;
float sy=sx*height/width;

Vector P_Cam;
P_Cam=Pe+(n2*d);

Vector P00;
P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));

for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
		int i = (y * width + x) * 3;	
Vector Pp,npe,P_hit,P_h;

Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

npe=Pp-Pe;
npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

Vector final_col;
int count;
final_col=castRay(Pe,npe,default_col);

plane_arr_f[i] = final_col.x;
plane_arr_f[i+1] = final_col.y;
plane_arr_f[i+2] = final_col.z;

			
		}
	}
stbi_flip_vertically_on_write(true);
stbi_write_jpg("sphere-fresnel.jpg",200,200,3,plane_arr_f,100);
}


// =============================================================================
// OPrnGL Display and Mouse Processing Functions.
//
// You can read up on OPrnGL and modify these functions, as well as the commands
// in main(), to Prrform more sophisticated display or GUI behavior. This code
// will service the bare minimum display needs for most assignments.
// =============================================================================
static void windowResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (w / 2), 0, (h / 2), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
static void windowDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE,plane_arr_f);
	glFlush();
}
static void processMouse(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
		exit(0);               // Exit on mouse click.
}
static void init(void)
{
	glClearColor(1, 1, 1, 1); // Set background color.
}

// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char *argv[])
{

	//initialize the global variables
	width = 200;
	height = 200;
	plane_arr_f = new unsigned char[200 * 200 * 3];

	setPixels();

	// OPrnGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefiniteL_y to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(200, 200); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Homework Zero");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; //This line never gets reached. We use it because "main" is T_yPr int.
}

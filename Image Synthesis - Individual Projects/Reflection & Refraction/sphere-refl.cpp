#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_include.h>

#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char *result;

int width = 200, height = 200, channels1, channels2, channels3;

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

float r=20,r2=30;


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

//Class for Sphere
class Sphere
{
public:
	Vector Pc;
	float r;
	Vector color;
	bool reflective;
	
	//intialize required members
	void set_values(float x,float y,float z, float radius, Vector &c,bool refl)
	{
		Pc.x=x;
		Pc.y=y;
		Pc.z=z;
		r=radius;
		color.x=c.x;
		color.y=c.y;
		color.z=c.z;
		reflective=refl;

	}
	//compute b for quadratic equation disc=b^2-4c
	float find_b(Vector &Pe,Vector &npe)
	{
		return npe*(Pe-Pc);

	}

	//compute normal at hitpoint
	Vector normal(Vector P)
	{
		Vector n=(P-Pc)*(1/r);
		return n;

	}
	
	//compute c for quadratic equation disc=b^2-4c
	float sphere_eq(Vector &P)
	{
	return (P-Pc)*(P-Pc) - pow(r,2);
	}

	

};



//array of sphere objects
Sphere sph[3];

//check ray-sphere intersection
int intersect(Vector &P, Vector &dir, Sphere &hit, float &t_hit)
	{
	
	for(int k=0;k<3;k++)
		{
		float b,c,disc;
		b=sph[k].find_b(P,dir);
		c=sph[k].sphere_eq(P);
		disc=sqrt(pow(b,2)-c);
		if((b>0 || isnan(disc)))
		{

		}
		else
		{
			t_hit=-b-disc;
			hit=sph[k];
		}

	}
	return t_hit!=-999;
}

//raycasting and color computation
Vector castRay(Vector &Pe, Vector &npe, Vector &default_col,const int &depth=0)
{

	Vector L;
	L.x=(0);
	L.y=(0);
	L.z=(-1);

	L=L*(1/magnitude(L.x,L.y,L.z));
	Vector dark;
	dark.x=0;
	dark.y=0;
	dark.z=0;

	Sphere sp;
	Vector hit_col;
	hit_col.x=0;
	hit_col.y=0;
	hit_col.z=0;
	
	float t_hit=-999;
	if(depth>3)
		return default_col;
	
	//if there is an intersection
	if(intersect(Pe,npe,sp,t_hit))
	{
		Vector P_hit=Pe+(npe*t_hit);
		Vector n_hit=sp.normal(P_hit);
		double T = 0.5*(L*n_hit) + 0.5;
		double S= 0.5*(n_hit.z*(n_hit.x+n_hit.y))+0.5;

		S=crop(0.5,1,S);
		Vector Sp;
		Sp.x=255;
		Sp.y=255;
		Sp.z=255;
		if(sp.reflective)
		{
			//reflection
			Vector n_r=(npe)- n_hit*(npe*n_hit)*2;
			hit_col=hit_col*(1-T) + dark*T+ castRay(P_hit, n_r,default_col,depth+1)*0.8;
			hit_col=hit_col*(1-S)+Sp*S;
		}
		else
			return sp.color;

	}
	else
		hit_col=default_col;
	return hit_col;
	
}


//setup
void setPixels()
{
	//base colors
	Vector sph1_c,sph2_c,sph3_c;
	sph1_c.x=205;
	sph1_c.y=205;
	sph1_c.z=255;

	sph2_c.x=255;
	sph2_c.y=255;
	sph2_c.z=0;

	sph3_c.x=255;
	sph3_c.y=0;
	sph3_c.z=0;

	//default color for no intersection
	Vector default_col;
	default_col.x=200;
	default_col.y=200;
	default_col.z=255;

	//Adding sphere objects
	sph[0].set_values(-150,40,60,30,sph1_c,true);
	sph[1].set_values(-110,80,60,20,sph2_c,false);
	sph[2].set_values(-100,40,80,20,sph3_c,false);


	//camera setup
	//camera up vector
	Vector Vup;
	Vup.x=0;
	Vup.y=1;
	Vup.z=0;

	//camera view direction
	Vector V_view;
	V_view.x=0;
	V_view.y=0;
	V_view.z=-1;

	//camera local normals
	Vector V0= cross_product(V_view,Vup);
	Vector n0;
	n0=V0*(1/magnitude(V0.x,V0.y,V0.z));
	Vector n2;
	n2=V_view*(1/magnitude(V_view.x,V_view.y,V_view.z));
	Vector n1= cross_product(n0,n2);

	//eyepoint
	Vector Pe;
	Pe.x=-100;
	Pe.y=40;
	Pe.z=220;

	//camera dimensions and distance from eyepoint
	float d=140,sx=150;
	float sy=sx*height/width;

	//center of camera
	Vector P_Cam;
	P_Cam=Pe+(n2*d);

	//bottom-left cornerof camera
	Vector P00;
	P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));

	//main raycasting loop
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;	
			Vector Pp,npe,P_hit,P_h;

			//point on camera plane
			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector final_col;
			final_col=castRay(Pe,npe,default_col);

			//output array
			result[i] = final_col.x;
			result[i+1] = final_col.y;
			result[i+2] = final_col.z;

				
		}
	}

	//write result to jpg
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("sphere-refl.jpg",200,200,3,result,100); 

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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE,result);
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
	result = new unsigned char[200 * 200 * 3];

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

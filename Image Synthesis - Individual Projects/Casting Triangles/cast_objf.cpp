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

#include "OBJ_Loader.h"

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include </home/user/Desktop/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include </home/user/Desktop/stb_image_write.h>
#include </home/user/Desktop/stb_include.h>
#include <string>
#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char *plane_arr_f;

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



void setPixels()
{

Vector Pc;
Pc.x=-100;
Pc.y=100;
Pc.z=-10;

Vector Vup;
Vup.x=1;
Vup.y=0;
Vup.z=0;

Vector V_view;
V_view.x=0;
V_view.y=-1;
V_view.z=1;

Vector V0= cross_product(V_view,Vup);
Vector n0;
n0=V0*(1/magnitude(V0.x,V0.y,V0.z));
Vector n2;
n2=V_view*(1/magnitude(V_view.x,V_view.y,V_view.z));

Vector n1= cross_product(n0,n2);

Vector Pe;
Pe.x=-2;
Pe.y=5;
Pe.z=-5;

float d=100,sx=200;
float sy=sx*height/width;

Vector P_Cam;
P_Cam=Pe+(n2*d);

Vector P00;
P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));


	objl::Loader Loader;

	// Load .obj File
	bool loadout = Loader.LoadFile("656_prism3.obj");

	// Check to see if it loaded

	// If so continue
	if (loadout)
	{

		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = Loader.LoadedMeshes[i];
			for (int j = 0; j < curMesh.Vertices.size()-3; j+=3)
			{
				Vector T_P0,T_P1,T_P2;

				T_P0.x=curMesh.Vertices[j].Position.X;
				T_P0.y=curMesh.Vertices[j].Position.Y;
				T_P0.z=curMesh.Vertices[j].Position.Z;

				T_P1.x=curMesh.Vertices[j+1].Position.X;
				T_P1.y=curMesh.Vertices[j+1].Position.Y;
				T_P1.z=curMesh.Vertices[j+1].Position.Z;

				T_P2.x=curMesh.Vertices[j+2].Position.X;
				T_P2.y=curMesh.Vertices[j+2].Position.Y;
				T_P2.z=curMesh.Vertices[j+2].Position.Z;


				Vector T_V0,T_V1,T_V2;

				T_V0=T_P0-T_P2;
				T_V1=T_P1-T_P0;
				T_V2=T_P2-T_P1;

				Vector A,T_n;
				A=cross_product(T_V0,T_V1);
				A=A*0.5;
				T_n=A*(1/magnitude(A.x,A.y,A.z));


				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						int i = (y * width + x) * 3;
							
				Vector Pp,npe,P_hit;

				Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

				npe=Pp-Pe;
				npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

				Vector T_A0,T_A1,T_A2;
				Vector T_n0,T_n1,T_n2;




				float num=(T_n*(T_P1-Pe));

				float den=(T_n*npe);

				if(num<0 && den<0)
				{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

				Vector L;
				L.x=(20);
				L.y=(0);
				L.z=(-1);
				L=L*(1/magnitude(L.x,L.y,L.z));
				double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;

				double S = 2 * (T_n.z * (T_n.x + T_n.y));
				double B = 1 - (Pe*T_n);

				T = crop(0, 1, T);
				S = crop(0, 1, S);
				B = crop(0, 1, B);


				plane_arr_f[i] = 255*(1-T)+255*(T);
				plane_arr_f[i+1] = 0*(1-T)+0*(T);
				plane_arr_f[i+2] = 0*(1-T)+255*(T);
				}
				}
				}
				}
			}
		}
	}

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

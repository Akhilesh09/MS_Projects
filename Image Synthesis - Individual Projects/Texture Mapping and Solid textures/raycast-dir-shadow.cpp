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

float sphere_eq(float a,float b,float c)
{

Vector P;
P.x=a;
P.y=b;
P.z=c;

Vector Pc;
Pc.x=-90;
Pc.y=100;
Pc.z=-10;

return (P-Pc)*(P-Pc) - pow(r,2);
}

void setPixels()
{
stbi_set_flip_vertically_on_load(true);
texture_img = stbi_load("map.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
			texture_arr[i] = texture_img[i];
			i++;
			texture_arr[i] = texture_img[i];
			i++;
			texture_arr[i] = texture_img[i];
			i++;
		}
	}

Vector Pc;
Pc.x=-100;
Pc.y=100;
Pc.z=-10;

Vector Vup;
Vup.x=180;
Vup.y=255;
Vup.z=50;
Vup=Vup*(1*magnitude(Vup.x,Vup.y,Vup.z));

Vector V_view;
V_view.x=30;
V_view.y=10;
V_view.z=200;
V_view=V_view*(1*magnitude(V_view.x,V_view.y,V_view.z));

Vector V0= cross_product(V_view,Vup);
Vector n0;
n0=V0*(1/magnitude(V0.x,V0.y,V0.z));
Vector n2;
n2=V_view*(1/magnitude(V_view.x,V_view.y,V_view.z));

Vector n1= cross_product(n0,n2);

Vector Pe;
Pe.x=-120;
Pe.y=100;
Pe.z=-200;

float d=150,sx=150;
float sy=sx*height/width;

Vector P_Cam;
P_Cam=Pe+(n2*d);

Vector P00;
P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));

for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
		int i = (y * width + x) * 3;
		plane_arr_f[i] = 255;
		plane_arr_f[i+1] = 255;
		plane_arr_f[i+2] = 0;
				}
	}

for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
		int i = (y * width + x) * 3;
			
Vector Pp,npe,P_hit;

Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

npe=Pp-Pe;
npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

Vector P_p1;
P_p1.x=-100;
P_p1.y=100;
P_p1.z=-80;
Vector n_p1=P_p1*(1/magnitude(P_p1.x,P_p1.y,P_p1.z));




float num=(n_p1*(P_p1-Pe));
float den=(n_p1*npe);
if(num<0 && den<0)
{
float t_hit=num/den;
P_hit=Pe+(npe*t_hit);


Vector L;
			L.x=(-100);
			L.y=(150);
			L.z=(120);
			L=L*(1/magnitude(L.x,L.y,L.z));
			double T = 0.5*(L.x*n_p1.x+ L.y*n_p1.y+L.z*n_p1.z) + 0.5;

			double S = 2 * (n_p1.z * (n_p1.x + n_p1.y));
			double B = 1 - (Pe*n_p1);

			T = crop(0, 1, T);
			S = crop(0, 1, S);
			B = crop(0, 1, B);

Vector Pl;
Pl.x=-100;
Pl.y=150;
Pl.z=120;

float b=L*((Pp-Pe)-P_hit);
float c=sphere_eq(P_hit.x,P_hit.y,P_hit.z);


float disc=sqrt(pow(b,2)-c);
if(b<0 && !isnan(disc))
{
	T=1;
	S=0;
}


plane_arr_f[i] = 255*(1-T)+0*(T);
plane_arr_f[i] = plane_arr_f[i]*(1-S)+255*(S);
 plane_arr_f[i] = plane_arr_f[i]*(1-B)+255*(B);
plane_arr_f[i+1] = 255*(1-T)+0*(T);
plane_arr_f[i+1] = plane_arr_f[i+1]*(1-S)+255*(S);
plane_arr_f[i+1] = plane_arr_f[i+1]*(1-B)+0*(B);
plane_arr_f[i+2] = 255*(1-T)+0*(T);
plane_arr_f[i+2] = plane_arr_f[i+2]*(1-S)+255*(S);
plane_arr_f[i+2] = plane_arr_f[i+2]*(1-B)+0*(B);
}

float b=npe*(Pe-Pc);
float c=sphere_eq(Pe.x,Pe.y,Pe.z);


float disc=sqrt(pow(b,2)-c);
if(b<0 && !isnan(disc))
{
float t_hit=-b-disc;
P_hit=Pe+(npe*t_hit);
Vector n_hit;
n_hit.x=(P_hit.x-Pc.x)/r;
n_hit.y=(P_hit.y-Pc.y)/r;
n_hit.z=(P_hit.z-Pc.z)/r;
Vector L;
			L.x=(-100);
			L.y=(150);
			L.z=(120);
			L=L*(1/magnitude(L.x,L.y,L.z));
			double T = 0.5*((L.x*n_hit.x) + (L.y*n_hit.y)+(L.z*n_hit.z)) + 0.5;

			double S = 2 * (n_hit.z * (n_hit.x + n_hit.y));
			double B = 1 - (Pe*n_hit);

			T = crop(0, 1, T);
			S = crop(0, 1, S);
			B = crop(0, 1, B);




plane_arr_f[i] = 0*(1-T)+0*(T);
plane_arr_f[i] = plane_arr_f[i]*(1-S)+255*(S);
plane_arr_f[i] = plane_arr_f[i]*(1-B)+255*(B);
plane_arr_f[i+1] = 255*(1-T)+0*(T);
plane_arr_f[i+1] = plane_arr_f[i+1]*(1-S)+255*(S);
plane_arr_f[i+1] = plane_arr_f[i+1]*(1-B)+0*(B);
plane_arr_f[i+2] = 255*(1-T)+0*(T);
plane_arr_f[i+2] = plane_arr_f[i+2]*(1-S)+255*(S);
plane_arr_f[i+2] = plane_arr_f[i+2]*(1-B)+0*(B);
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

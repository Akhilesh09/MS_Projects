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
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <string>
#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
//int width, height;
unsigned char *texture_arr;
unsigned char *texture_img;

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

float r=200;


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

float Plane_eq1(float a,float b,float c)
{

Vector P;
P.x=a;
P.y=b;
P.z=c;

	Vector P_p1;
P_p1.x=100;
P_p1.y=100;
P_p1.z=0;
// Vector n_p1=P_p1*(1/magnitude(P_p1.x,P_p1.y,P_p1.z));


Vector n_p1;
n_p1.x=0;
n_p1.y=1;
n_p1.z=1;
n_p1=n_p1*(1/magnitude(n_p1.x,n_p1.y,n_p1.z));

	return n_p1 *(P-P_p1);
}

float sphere_eq(float a,float b,float c)
{

Vector P;
P.x=a;
P.y=b;
P.z=c;

Vector Pc;
Pc.x=-90;
Pc.y=200;
Pc.z=-10;

return (P-Pc)*(P-Pc) - pow(r,2);
}

void setPixels()
{
	stbi_set_flip_vertically_on_load(true);
	texture_img = stbi_load("texture2.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			texture_arr[i] = texture_img[i];
			i++;
			texture_arr[i] = texture_img[i];
			i++;
			texture_arr[i] = texture_img[i];
			i++;
		}
	}


Vector Vup;
Vup.x=0;
Vup.y=1;
Vup.z=0;

Vector V_view;
V_view.x=0;
V_view.y=0;
V_view.z=1;

Vector V0= cross_product(V_view,Vup);
Vector n0;
n0=V0*(1/magnitude(V0.x,V0.y,V0.z));
Vector n2;
n2=V_view*(1/magnitude(V_view.x,V_view.y,V_view.z));

Vector n1= cross_product(n0,n2);

Vector Pe;
Pe.x=0;
Pe.y=0;
Pe.z=0;

float d=26,sx=10;
float sy=sx*height/width;

Vector P_Cam;
P_Cam=Pe+(n2*d);

Vector P00;
P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));

Vector P_p1;
P_p1.x=0;
P_p1.y=-25;
P_p1.z=0;


Vector n_p1;
n_p1.x=0;
n_p1.y=1;
n_p1.z=0;
n_p1=n_p1*(1/magnitude(n_p1.x,n_p1.y,n_p1.z));

// Vector n_v2;
// n_v2.x=-25;
// n_v2.y=120;
// n_v2.z=-500;
// n_v2=n_v2*(1/magnitude(n_v2.x,n_v2.y,n_v2.z));

// Vector n_p2=cross_product(n_p1,n_v2);

// Vector n_p3=cross_product(n_p1,n_p2);

// float p_sx=30,p_sy=p_sx*height/width;


for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
		int i = (y * width + x) * 3;
			
Vector Pp,npe,P_hit;

Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

npe=Pp-Pe;
npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

float num=(n_p1*(P_p1-Pe));
float den=(n_p1*npe);
// cout<<num<<endl<<den<<endl;
if(num<0 && den<0)
{
float t_hit=num/den;
cout<<t_hit<<endl;
P_hit=Pe+(npe*t_hit);
int j=((int)Pp.y*width+(int)Pp.x)*3;
int k=((int)P_hit.y*width+(int)P_hit.x)*3;
Vector L;
// L=npe;
			L.x=(-1);
			L.y=(-1);
			L.z=(-1);
			// L=L*(1/magnitude(L.x,L.y,L.z));
			double T = 0.5*(L.x*n_p1.x+ L.y*n_p1.y+L.z*n_p1.z) + 0.5;

			double S = 2 * (n_p1.z * (n_p1.x + n_p1.y));
			double B = 1 - (Pe*n_p1);

			T = crop(0, 1, T);
			S = crop(0, 1, S);
			B = crop(0, 1, B);
// cout<<T<<endl;
// float u= (n_p2*(P_hit-P_p1))/p_sx;
// float v= (n_p3*(P_hit-P_p1))/p_sy;

plane_arr_f[i] = 255*(1-T)+0*(T);
plane_arr_f[i] = plane_arr_f[i]*(1-S)+255*(S);
plane_arr_f[i] = plane_arr_f[i]*(1-B)+255*(B);
plane_arr_f[i+1] = 255*(1-T)+0*(T);
plane_arr_f[i+1] = plane_arr_f[i+1]*(1-S)+255*(S);
plane_arr_f[i+1] = plane_arr_f[i+1]*(1-B)+0*(B);
plane_arr_f[i+2] = 255*(1-T)+0*(T);
plane_arr_f[i+2] = plane_arr_f[i+2]*(1-S)+255*(S);
plane_arr_f[i+2] = plane_arr_f[i+2]*(1-B)+0*(B);


// if(u>0 && u<1 && v>0 && v<1){
// 	float x=u*width;
// 	float y=v*height;
// 	int I=floor(x+0.5);
// 	int J=floor(y+0.5);

// int k=(int)((J*width+I)*3);
// plane_arr_f[j] = texture_arr[k];//*(1-T)+0*(T);
// // plane_arr_f[j] = plane_arr_f[j]*(1-S)+255*(S);
// //  plane_arr_f[j] = plane_arr_f[j]*(1-B)+255*(B);
// plane_arr_f[j+1] = texture_arr[k+1];//*(1-T)+0*(T);
// // plane_arr_f[j+1] = plane_arr_f[j+1]*(1-S)+255*(S);
// // plane_arr_f[j+1] = plane_arr_f[j+1]*(1-B)+0*(B);
// plane_arr_f[j+2] = texture_arr[k+2];//*(1-T)+0*(T);
// // plane_arr_f[j+2] = plane_arr_f[j+2]*(1-S)+255*(S);
// // plane_arr_f[j+2] = plane_arr_f[j+2]*(1-B)+0*(B);
//}
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
	texture_arr = new unsigned char[200 * 200 * 3];
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

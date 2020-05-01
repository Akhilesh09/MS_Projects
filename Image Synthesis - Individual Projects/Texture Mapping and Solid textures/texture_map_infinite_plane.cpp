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


};

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

//setup
void setPixels()
{

	//load texture image
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

	//sphere center
	Vector Pc;
	Pc.x=-100;
	Pc.y=100;
	Pc.z=-10;
	
	//camera setup
	//camera up vector

	Vector Vup;
	Vup.x=150;
	Vup.y=15;
	Vup.z=-100;

	//camera view direction
	Vector V_view;
	V_view.x=10;
	V_view.y=10;
	V_view.z=100;

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
	Pe.y=100;
	Pe.z=-200;

	//camera dimesnsions and distance from eyepoint
	float d=140,sx=150;
	float sy=sx*height/width;

	//center of camera
	Vector P_Cam;
	P_Cam=Pe+(n2*d);

	//bottom-left corner of camera
	Vector P00;
	P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));
	
	//point on plane
	Vector P_p1;
	P_p1.x=-100;
	P_p1.y=100;
	P_p1.z=-80;
	Vector n_p1=P_p1*(1/magnitude(P_p1.x,P_p1.y,P_p1.z));

	Vector n_v2;
	n_v2.x=25;
	n_v2.y=-60;
	n_v2.z=50;
	n_v2=n_v2*(1/magnitude(n_v2.x,n_v2.y,n_v2.z));


	Vector n_p2=cross_product(n_p1,n_v2);

	Vector n_p3=cross_product(n_p1,n_p2);

	//dimensions for repeating texture
	float p_sx=30,p_sy=p_sx*height/width;




	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
			
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			float num=(n_p1*(P_p1-Pe));
			float den=(n_p1*npe);

			if(num<0 && den<0)
			{

				float t_hit=num/den;
				P_hit=Pe+(npe*t_hit);

				float u= (n_p2*(P_hit-P_p1))/p_sx;
				float v= (n_p3*(P_hit-P_p1))/p_sy;


				if(u<0)
				{
					u=u-floor(u);
					u=1-u;
				}
				else
				{
					u=u-floor(u);
				}
				if(v<0)
				{
					v=v-floor(v);
					v=1-v;
				}
				else
				{
					v=v-floor(v);
				}

				float x=u*width;
				float y=v*height;
				int I=floor(x+0.5);
				int J=floor(y+0.5);

				int k=(int)((J*width+I)*3);
				result[i] = texture_arr[k];
				result[i+1] = texture_arr[k+1];
				result[i+2] = texture_arr[k+2];

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

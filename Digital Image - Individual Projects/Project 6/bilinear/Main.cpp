#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

unsigned char *org_img;
unsigned char *image1;

unsigned char *out_img;
int width = 600, height = 600, channels1, channels2;

void setPixels()
{
	
	//initilaize array with input image
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("cheetah.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind++] = image1[org_ind++];
			org_img[org_ind++] = image1[org_ind++];
			org_img[org_ind] = image1[org_ind];
		}
	}
	
	//initilaize background color
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int out_ind = (y * width + x) * 3;
			out_img[out_ind++] = 255;
			out_img[out_ind++] = 255;
			out_img[out_ind] = 255;
		}
	}

	//normalizing coordinates between 0 and 1

	
	
	float x0 = 30/ 600.0;
	float y0 = 100/ 600.0;
	float x1 = 200 / 600.0;
	float y1 = 600 / 600.0;
	float x2 = 400 / 600.0;
	float y2 = 600 / 600.0;
	float x3 = 500 / 600.0;
	float y3 = 300 / 600.0;

	//bilinear transform
	
	float a0 = x0;
	float a1 = x3 - x0;
	float a2 = x1 - x0;
	float a3 = x2 - x1 - x3 - x0;
	float b0 = y0;
	float b1 = y3 - y0;
	float b2 = y1 - y0;
	float b3 = y2 - y1 - y3 - y0;



	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int out_ind = (j * width + i) * 3;

			float y = j / 600.0;
			float x= i / 600.0;
			float u, v;
			float c0=a1*(b0-y)+b1*(x-a0);
			float c1= (a3 * (b0 - y))+( b3 * (x-a0))+(a1*b2)-(a2*b1);
			float c2=(a3*b2)-(a2*b3);
			float disc = sqrt(c1*c1 - 4.0*c2*c0);

			v = (-c1 + disc) / (2.0*c2);
			u = (x - a0 - a2 * v) / (a1 + a3 * v);

			//converting the coordinates to be between 0-600
			
			u *= 600;
			v *= 600;
			u = (int)u % width;
			v = (int)v  % height;

			if ((u > 0) && (v > 0) && (u < width - 1) && (v < height - 1))
			{
				int org_ind = (v * width + u) * 3;
				out_img[out_ind++] = org_img[org_ind++];
				out_img[out_ind++] = org_img[org_ind++];
				out_img[out_ind] = org_img[org_ind];
			}
		}
	}
}


// =============================================================================
// OpenGL Display and Mouse Processing Functions.
//
// You can read up on OpenGL and modify these functions, as well as the commands
// in main(), to perform more sophisticated display or GUI behavior. 
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, out_img);
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
	width = 600;
	height = 600;
	org_img = new unsigned char[600 * 600 * 3];
	out_img = new unsigned char[600 * 600 * 3];



	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Project Six");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}


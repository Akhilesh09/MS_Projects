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

	//rotate transform u=x*cos(theta) + y*sin(theta), v=y*cos(theta) - x*sin(theta)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			int u = x * cos(50) + y * sin(50);
			int v = y * cos(50) - x * sin(50);


			if ((u > 0) && (v > 0) && (u < width - 1) && (v < height - 1))
			{
				int l = (v * width + u) * 3;
				out_img[org_ind++] = org_img[out_ind++];
				out_img[org_ind++] = org_img[out_ind++];
				out_img[org_ind] = org_img[out_ind];
			}
		}
	}
}

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
	glutCreateWindow("Rotate Transform");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; 
}


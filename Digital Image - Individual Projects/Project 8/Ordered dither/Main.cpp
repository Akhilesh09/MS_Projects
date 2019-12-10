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
unsigned char *inp_img;

double *org_real_img;

int width = 300, height = 300, channels;


void setPixels()
{

	int dither_ctrl[4][4] = { {16,144,48,176},{208,80,240,112},{64,192,32,160},{256,128,224,96 } };


	stbi_set_flip_vertically_on_load(true);
	inp_img = stbi_load("org.png", &width, &height, &channels, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind] = inp_img[org_ind++];
			org_img[org_ind] = inp_img[org_ind++];
			org_img[org_ind] = inp_img[org_ind++];
		}
	}
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_real_ind = (y * width + x) * 3;
			org_real_img[org_real_ind] = (double)org_img[org_real_ind];
			org_real_ind++;
			org_real_img[org_real_ind] = (double)org_img[org_real_ind];
			org_real_ind++;
			org_real_img[org_real_ind] = (double)org_img[org_real_ind];
			org_real_ind++;
		}
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_real_ind = (y * width + x) * 3;
			if (org_real_img[org_real_ind] < dither_ctrl[x % 4][y %4] ) 
				org_real_img[org_real_ind] = 0;
				else
					org_real_img[org_real_ind] = 255;
			org_real_ind++;
			if (org_real_img[org_real_ind] < dither_ctrl[x % 4][y % 4])
				org_real_img[org_real_ind] = 0;
				else
					org_real_img[org_real_ind] = 255;
			org_real_ind++;
			if (org_real_img[org_real_ind] < dither_ctrl[x % 4][y % 4])
				org_real_img[org_real_ind] = 0;
				else
					org_real_img[org_real_ind] = 255;

		}
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind] = org_real_img[org_ind];
			org_ind++;
			org_img[org_ind] = org_real_img[org_ind];
			org_ind++;
			org_img[org_ind] = org_real_img[org_ind];
			org_ind++;
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
	glDrawPixels(300, 300, GL_RGB, GL_UNSIGNED_BYTE, org_img);
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
	width = 300;
	height = 300;
	org_img = new unsigned char[300 * 300 * 3];
	org_real_img = new double[300 * 300 * 3];

	setPixels();

	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Ordered Dither");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

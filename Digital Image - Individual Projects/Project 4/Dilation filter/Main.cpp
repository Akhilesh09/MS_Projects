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

unsigned char *output_img;
double *kernel;
double filter_arr[9] = {1,1,1,1,1,1,1,1,1 };
int width = 300, height = 168, channels;

int maximum(int a, int b) {
	return a>b?a:b;
}

void setPixels()
{
	stbi_set_flip_vertically_on_load(true);
	inp_img = stbi_load("forest.jpg", &width, &height, &channels, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind++] = inp_img[org_ind++] ;
			org_img[org_ind++] = inp_img[org_ind++] ;
			org_img[org_ind] = inp_img[org_ind];
		}
	}

	int filter_ind = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			int kernel_ind = (y * 3 + x) * 3;
			kernel[kernel_ind++] = filter_arr[filter_ind];
			kernel[kernel_ind++] = filter_arr[filter_ind];
			kernel[kernel_ind] = filter_arr[filter_ind];
			filter_ind = filter_ind + 1;
		}
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double r = 0, g = 0, b = 0;
			for (int i = -1; i <= +1; i++) {
				for (int j = -1; j <= +1; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newy > height - 1)
						newy = (height) - (newy % (height)) - 2;
					else if (newy < 0) {
						newy = (height)-(newy % (height)) - 2;
					}
					if (newx > width - 1)
						newx = (width) - (newx % (width)) - 2;
					else if (newx < 0) {
						newx = (width)-(newx % (width)) - 2;
					}
					int kernel_ind = ((i + 1) * 3 + (j + 1)) * 3;
					int org_ind = (newy * width + newx) * 3;
					r = maximum(r,org_img[org_ind++] * kernel[kernel_ind++]);
					g = maximum(g,org_img[org_ind++] * kernel[kernel_ind++]);
					b = maximum(b,org_img[org_ind++] * kernel[kernel_ind++]);
				}
			}

			int output_ind = (y * width + x) * 3;
			output_img[output_ind++] = r;
			output_img[output_ind++] = g;
			output_img[output_ind] = b;
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, output_img);
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
	height = 168;
	org_img = new unsigned char[300 * 300 * 3];
	output_img = new unsigned char[300 * 300 * 3];
	kernel = new double[3 * 3*3];



	setPixels();

	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Dilation Filter");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

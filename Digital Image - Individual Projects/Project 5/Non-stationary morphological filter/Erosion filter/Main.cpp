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

unsigned char *ctrl_img;
unsigned char *image2;

double *kernel;
unsigned char *out_img;
int width = 300, height = 300, channels1, channels2;

int minimum(int a, int b) {
	return a < b ? a : b;
}

void createkernel(int x,int y)
{
			for (int i = -1; i <= +1; i++) {
				for (int j = -1; j <= +1; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newx > height - 1)
					{
						newx = (height)-(height - abs(newx) % height) - 1;
					}
					else if (newx < 0) {
						newx = (height)-(height - abs(newx) % height) - 1;
					}
					if (newy > width - 1)
					{
						newy = (width)-(width - abs(newy) % (width)) - 1;
					}
					else if (newy < 0)
					{
						newy = (width)-(width - abs(newy) % (width)) - 1;
					}

					long ctrl_ind = (newx * width + newy) * 3;
					long kernel_ind = ((i +1) * 3 + (j + 1)) * 3;
					kernel[kernel_ind++] = ctrl_img[ctrl_ind++]/255.0;
					kernel[kernel_ind++] = ctrl_img[ctrl_ind++]/255.0;
					kernel[kernel_ind] = ctrl_img[ctrl_ind]/255.0;
		}
	}
}


void setPixels()
{
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("forest2.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind] = image1[org_ind++];
			org_img[org_ind] = image1[org_ind++];
			org_img[org_ind] = image1[org_ind];
		}
	}
	image2 = stbi_load("ctrl8.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int ctrl_ind = (y * width + x) * 3;
			ctrl_img[ctrl_ind] = image2[ctrl_ind++];
			ctrl_img[ctrl_ind] = image2[ctrl_ind++];
			ctrl_img[ctrl_ind] = image2[ctrl_ind];
		}
	}

	for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
			double r = 255, g = 255, b = 255;
			int index = (y * width + x) * 3;
			createkernel(x,y);
			for (int i = -1; i <= +1; i++) {
				for (int j = -1; j <= +1; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newy > height - 1) 
					{
						newy = (height)-(height - abs(newy) % height) - 1;
					}
					else if (newy < 0) {
						newy = (height)-(height-abs(newy) % height) - 1;
					}
					if (newx > width - 1)
					{
						newx = (width)-(width - abs(newx) % (width)) - 1;
					}
					else if (newx < 0) 
					{
						newx = (width)-(width-abs(newx) % (width)) - 1;
					}
					long color = ((i + 1) * 3 + (j + 1)) * 3;
					long a = (newy * width + newx) * 3;
					r = minimum(r, org_img[a++] * kernel[color++]);
					g = minimum(g, org_img[a++] * kernel[color++]);
					b = minimum(b, org_img[a] * kernel[color]);
				}
			}
			out_img[index++] = r;
			out_img[index++] = g;
			out_img[index] = b;
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
	width = 300;
	height = 300;
	org_img = new unsigned char[300 * 300 * 3];
	ctrl_img = new unsigned char[300 * 300 * 3];
	kernel = new double[3 * 3 * 3];
	out_img = new unsigned char[300 * 300 * 3];


	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Control Image Erosion");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

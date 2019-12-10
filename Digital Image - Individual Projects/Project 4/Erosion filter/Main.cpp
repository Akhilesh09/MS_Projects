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

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
unsigned char *org_img;
unsigned char *image1;

unsigned char *out_img;
double *kernel;
double filter_arr[9] = {1,1,1,1,1,1,1,1,1 };
int width = 300, height = 168, channels1;

// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "org_img."
// Then, "glutDisplayFunc" below will use org_img to display the pixel colors.
// =============================================================================
int minimum(int a, int b) {
	return a<b?a:b;
}


void setPixels()
{
	
	//initilaize array with input image
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("forest.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			org_img[index++] = image1[index++] ;
			org_img[index++] = image1[index++] ;
			org_img[index] = image1[index];
		}
	}

	//initilaize kernel with convolutional filter values
	int filter_ind = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			int index = (y * 3 + x) * 3;
			kernel[index++] = filter_ar[filter_ind];
			kernel[index++] = filter_arr[filter_ind];
			kernel[index] = filter_arr[filter_ind];
			filter_ind = filter_ind + 1;
		}
	}
	
	// applying kernel to input image 
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double r = 255, g = 255, b = 255;
			for (int i = -1; i <= +1; i++) {
				for (int j = -1; j <= +1; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newy > height - 1)
						newy = (height) - (newy % (height)) - 1;
					else if (newy < 0)
						newy = (height) - (newy % (height)) - 1;
					if (newx > width - 1)
						newx = (width) - (newx % (width)) - 1;
					else if (newx < 0)
						newx = (width) - (newx % (width)) - 1;
					int kernel_ind = ((i + 1) * 3 + (j + 1)) * 3;
					int org_ind = (newy * width + newx) * 3;
					r = minimum(r,org_img[org_ind++] * kernel[kernel_ind++]);
					g = minimum(g,org_img[org_ind++] * kernel[kernel_ind++]);
					b = minimum(b,org_img[org_ind++] * kernel[kernel_ind++]);
				}
			}

			// storing result of convolution in output array
			int index = (y * width + x) * 3;
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
	height = 168;
	org_img = new unsigned char[300 * 300 * 3];
	out_img = new unsigned char[300 * 300 * 3];
	kernel = new double[3 * 3*3];



	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Erosion Filter");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

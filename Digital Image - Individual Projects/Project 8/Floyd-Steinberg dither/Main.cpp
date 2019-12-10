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

double *org_nrmlzd_img;

int width = 300, height = 300, channels1, channels2,channels3;

int maximum(int a, int b)
{
	return a > b ? a : b;
}

void setPixels()
{
	
	//initilaize array with input image
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("org.png", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind] = image1[org_ind++];
			org_img[org_ind] = image1[org_ind++];
			org_img[org_ind] = image1[org_ind++];
		}
	}
	
	//initilaize array with normalized pixels values
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_nrmlzd_ind = (y * width + x) * 3;
			org_nrmlzd_img[org_nrmlzd_ind] = (double)org_img[org_nrmlzd_ind]/255.0;
			org_nrmlzd_ind++;
			org_nrmlzd_img[org_nrmlzd_ind] = (double)org_img[org_nrmlzd_ind] / 255.0;
			org_nrmlzd_ind++;
			org_nrmlzd_img[org_nrmlzd_ind] = (double)org_img[org_nrmlzd_ind] / 255.0;
			org_nrmlzd_ind++;
		}
	}
	
	//floyd-steinberg dither
	/*
	
	for each y from top to bottom
    for each x from left to right
      oldpixel  := pixel[x][y]
      newpixel  := find_closest_palette_color(oldpixel)
      pixel[x][y]  := newpixel
      quant_error  := oldpixel - newpixel
      pixel[x + 1][y    ] := pixel[x + 1][y    ] + quant_error * 7 / 16
      pixel[x - 1][y + 1] := pixel[x - 1][y + 1] + quant_error * 3 / 16
      pixel[x    ][y + 1] := pixel[x    ][y + 1] + quant_error * 5 / 16
      pixel[x + 1][y + 1] := pixel[x + 1][y + 1] + quant_error * 1 / 16
	
	*/
	for (int y = 1; y < height-1; y++) {
		for (int x = 0; x < width; x++) {
			int org_nrmlzd_ind = (y * width + x) * 3;

			float r=(int)(org_nrmlzd_img[org_nrmlzd_ind]+0.5);
			float g=(int)(org_nrmlzd_img[org_nrmlzd_ind+1] + 0.5);
			float b=(int)(org_nrmlzd_img[org_nrmlzd_ind+2] + 0.5);


			float err_r, err_g, err_b;
			err_r = org_nrmlzd_img[i]-r;
			err_g = org_nrmlzd_img[i+1]-g;
			err_b = org_nrmlzd_img[i+2]-b;

			int newy = y;
			int newx = x+1; 

			int index = (c*width + d ) * 3;
			org_nrmlzd_img[index] += (er * 0.4375);
			org_nrmlzd_img[index+1] += (eg * 0.4375);
			org_nrmlzd_img[index+2] += (eb * 0.4375);

			newy = y+1;
			newx = x-1;

			index = (c*width + d) * 3;
			org_nrmlzd_img[index] += (er * 0.1875);
			org_nrmlzd_img[index+1] += (eg * 0.1875);
			org_nrmlzd_img[index+2] += (eb * 0.1875);

			newy = y+1;
			newx = x;

			index = (c*width + d) * 3;
			org_nrmlzd_img[index] += (er * 0.3125);
			org_nrmlzd_img[index+1] += (eg * 0.3125);
			org_nrmlzd_img[index+2] += (eb * 0.3125);

			newy = y + 1;
			newx = x+1;

			index = (c*width + d) * 3;
			org_nrmlzd_img[index] += (er * 0.0625);
			org_nrmlzd_img[index+1] += (eg * 0.0625);
			org_nrmlzd_img[index+2] += (eb * 0.0625);
		}
	}

	//convert normalized pixels values to 0-255
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind] = org_nrmlzd_img[org_ind] * 255;
			org_ind++;
			org_img[org_ind] = org_nrmlzd_img[org_ind] * 255;
			org_ind++;
			org_img[org_ind] = org_nrmlzd_img[org_ind] * 255;
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
	org_nrmlzd_img = new double[300 * 300 * 3];

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Floyd Steinberg Dither");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

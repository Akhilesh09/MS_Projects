#include <cstdlib>
#include <iostream>
#include <GL/glut.h>

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_inp_img_IMPLEMENTATION
#include <stb_inp_img.h>


using namespace std;

// =============================================================================
// These variables will store the input ppm inp_img's width, height, and color
// =============================================================================
//int width, height;
unsigned char *org_img;
unsigned char *inp_img;
int width, height, channels;
double piecewise_x[4] = { 0,0.25,0.5,1 }, piecewise_y[4] = {0,0.2,0.7,1};

// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "org_img."
// Then, "glutDisplayFunc" below will use org_img to display the pixel colors.
// =============================================================================


//returning y value on corresponding piecewise linear curve for given x value
double line_slopeeq(double x,int i)
{
	double y2 = piecewise_y[i];
	double y1 = piecewise_y[i-1];
	double x2 = piecewise_x[i];
	double x1 = piecewise_x[i-1];
	double m = (y2 - y1) / (x2 - x1);
	return m*(x-x1)+y1;
}

//manipulates R G or B values of image based on return value of function "line_slopeeq" 
void piecewise(int org_x,int channel_ind)
{	
		double img_x= double(org_x) / width;
		if (img_x==0 )
		{
			org_img[channel_ind] = 0; // use channel_ind+1 for green channel, channel_ind+2 for blue channel
		}
		else if( img_x==1)
		{
			org_img[channel_ind] = 255; // use channel_ind+1 for green channel, channel_ind+2 for blue channel
		}
		else
		{
			int curve_ind = 0;
			while (curve_ind < 4) {
				if (img_x > piecewise_x[curve_ind] && img_x <= piecewise_x[curve_ind + 1])
				{
					org_img[channel_ind] = line_slopeeq(img_x, curve_ind + 1) * 255; // use i+1 for green channel, i+2 for blue channel
					break;
				}
				j++;
			}
		}
}

void setPixels()
{
	//initilaize array with input image
	stbi_set_flip_vertically_on_load(true);
	inp_img = stbi_load("tree.jpg",&width,&height,&channels, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			org_img[index++] = inp_img[index++];
			org_img[index++] = inp_img[index++];
			org_img[index] = inp_img[index];
		}
	}
	
	//linear piecewise curve manipulation
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			piecewise(x,index);
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, org_img);
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
	org_img = new unsigned char[300 * 300 * 3];

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Image Manipulation");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; 
}

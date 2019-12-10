#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#define maximum(x, y, z) ((x) > (y)? ((x) > (z)? (x) : (z)) : ((y) > (z)? (y) : (z)))
#define minimum(x, y, z) ((x) < (y)? ((x) < (z)? (x) : (z)) : ((y) < (z)? (y) : (z)))

using namespace std;

//int width, height;
unsigned char *org_img;
unsigned char *image1;

unsigned char *hue_img;
unsigned char *image2;

int width=300, height=168, channels1, channels2;

// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "org_img."
// Then, "glutDisplayFunc" below will use org_img to display the pixel colors.
// =============================================================================


	void RGBtoHSV(int r, int g, int b, double &h, double &s, double &v) {

		double red, green, blue;
		double max, min, delta;

		red = r / 255.0; green = g / 255.0; blue = b / 255.0;  /* r, g, b to 0 - 1 scale */

		max = maximum(red, green, blue);
		min = minimum(red, green, blue);

		v = max;        /* value is maximum of r, g, b */

		if (max == 0) {    /* saturation and hue 0 if value is 0 */
			s = 0;
			h = 0;
		}
		else {
			s = (max - min) / max;           /* saturation is color purity on scale 0 - 1 */

			delta = max - min;
			if (delta == 0)                    /* hue doesn't matter if saturation is 0 */
				h = 0;
			else {
				if (red == max)                  /* otherwise, determine hue on scale 0 - 360 */
					h = (green - blue) / delta;
				else if (green == max)
					h = 2.0 + (blue - red) / delta;
				else /* (blue == max) */
					h = 4.0 + (red - green) / delta;
				h = h * 60.0;
				if (h < 0)
					h = h + 360.0;
			}
		}
	}



	void HSVtoRGB(double h, double s, double v, int &r, int &g, int &b) {
		double C = s * v;
		double X = C * (1 - abs(fmod(h / 60.0, 2) - 1));
		double m = v - C;
		double Rs, Gs, Bs;

		if (h >= 0 && h < 60) {
			Rs = C;
			Gs = X;
			Bs = 0;
		}
		else if (h >= 60 && h < 120) {
			Rs = X;
			Gs = C;
			Bs = 0;
		}
		else if (h >= 120 && h < 180) {
			Rs = 0;
			Gs = C;
			Bs = X;
		}
		else if (h >= 180 && h < 240) {
			Rs = 0;
			Gs = X;
			Bs = C;
		}
		else if (h >= 240 && h < 300) {
			Rs = X;
			Gs = 0;
			Bs = C;
		}
		else {
			Rs = C;
			Gs = 0;
			Bs = X;
		}
		r = (Rs+m) * 255 ;
		g = (Gs+m) * 255;		
		b = (Bs+m) * 255;
	}


void setPixels()
{
	//initilaize arrays with input images
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("forest.jpg", &width, &height, &channels1, STBI_rgb);
	image2 = stbi_load("ctrl.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			org_img[index++] = image1[index++];
			org_img[index++] = image1[index++];
			org_img[index] = image1[index];
			hue_img[index++] = image2[index++];
			hue_img[index++] = image2[index++];
			hue_img[index] = image2[index];
		}
	}

    //Hues Replacement
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			double h2,s2,v2,h1, s1, v1;
			int r1, g1, b1, r2, g2, b2;
			RGBtoHSV((int)org_img[i], (int)org_img[i + 1], (int)org_img[i + 2], h1, s1, v1);
			RGBtoHSV((int)hue_img[i], (int)hue_img[i+1], (int)hue_img[i+2], h2, s2, v2);
			h1 = h2;
			s1 = s2;
			HSVtoRGB(h2, s2, v2, r2, g2, b2);
			HSVtoRGB(h1, s1, v1,r1,g1,b1);
			org_img[i] = r1;
			org_img[i + 1] = g1;
			org_img[i + 2] = b1;
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

	//initialize the global variables
	width = 300;
	height = 168;
	org_img = new unsigned char[300 * 300 * 3];
	hue_img = new unsigned char[300 * 300 * 3];

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Replace Hues");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; 
}

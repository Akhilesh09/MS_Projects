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
//int width, height;
unsigned char *org_img;
unsigned char *image1;

unsigned char *control_img;
unsigned char *image2;

double *kernel;
unsigned char *output_img;
int width = 300, height = 300, channels1, channels2;

//
double eq(int x, int y, double h)
{
	return 2 * abs((cos(h)*x + sin(h)*y - (cos(h)*2.5) - (sin(h)* 2.5))) - 2;
}

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

void createkernel(double h)
{
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			float ri = rand() % 1;
			float rj = rand() % 1;
			int r = 0;
			int g = 0;
			int b = 0;
			for (float m = 0; m < 9; m++)
			{
				for (float n = 0; n < 9; n++)
				{
					if (eq(x + m / 9 + ri / 9, y + n / 9 + rj / 9, h) <= 0)
					{
						r += 1;
						g += 1;
						b += 1;
					}
					else {
						r += 0;
						g += 0;
						b += 0;
					}
				}
			}
			r /= 81.0;
			g /= 81.0;
			b /= 81.0;
			int kernel_ind = (y * 5 + x) * 3;
			kernel[kernel_ind++] = r;
			kernel[kernel_ind++] = g;
			kernel[kernel_ind] = b;
		}
	}
	double sumr = 0, sumg = 0, sumb = 0;

	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			int kernel_ind = (y * 5 + x) * 3;
			sumr += kernel[kernel_ind++];
			sumg += kernel[kernel_ind++];
			sumb += kernel[kernel_ind];
		}
	}

	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			int kernel_ind = (y * 5 + x) * 3;
			kernel[kernel_ind] = (int)kernel[kernel_ind++] / sumr;
			kernel[kernel_ind] = (int)kernel[kernel_ind++] / sumg;
			kernel[kernel_ind] = (int)kernel[kernel_ind++] / sumb;
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
	image2 = stbi_load("skies.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int ctrl_ind  = (y * width + x) * 3;
			control_img[ctrl_ind] = image2[ctrl_ind++];
			control_img[ctrl_ind] = image2[ctrl_ind++];
			control_img[ctrl_ind] = image2[ctrl_ind];
		}
	}
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double r = 0, g = 0, b = 0;
			double h, s, v;
			int index = (y * width + x) * 3;
			RGBtoHSV((int)control_img[k], (int)control_img[k+1], (int)control_img[k+2], h, s, v);
			createkernel(h);
			for (int i = -2; i <= +2; i++) {
				for (int j = -2; j <= +2; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newy > height - 1)
						newy = (height)-(newy % height) - 1;
					else if (newy < 0) {
						newy = (height)-(newy % (height)) - 1;
					}
					if (newx > width - 1)
						newx = (width)-(newx % (width)) - 1;
					else if (newx < 0) {
						newx = (width)-(newx % (width)) - 1;
					}
					long kernel_ind = ((i + 2) * 5 + (j + 2)) * 3;
					long org_ind = (newy * width + newx) * 3;
					r += org_img[org_ind++] * kernel[kernel_ind++];
					g += org_img[org_ind++] * kernel[kernel_ind++];
					b += org_img[org_ind] * kernel[kernel_ind];
				}
			}


			output_img[index++] = r;
			output_img[index++] = g;
			output_img[index] = b;
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
	height = 300;
	org_img = new unsigned char[300 * 300 * 3];
	control_img = new unsigned char[300 * 300 * 3];
	kernel = new double[5 * 5 * 3];
	output_img = new unsigned char[300 * 300 * 3];


	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Non-Stationary Blur");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

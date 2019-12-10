#include <cstdlib>
#include <iostream>
#include <GL/glut.h>

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
int width, height;
unsigned char *convex_poly;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "convex_poly."
// Then, "glutDisplayFunc" below will use convex_poly to display the pixel colors.
// =============================================================================

//returns true if pixel lies within convex polygon shape and false otherwise.
int shape(float x, float y)
{

	double x1 = 150;
	double y1 = 150;
	double n0x1 = cos(70);
	double n0y1 = sin(70);
	double L0x1 = 150 + 50 * n0x1;
	double L0y1 = 150 + 50 * n0y1;

	double x2 = 150;
	double y2 = 150;
	double n0x2 = cos(80);
	double n0y2 = sin(80);
	double L0x2 = 150 + 50 * n0x2;
	double L0y2 = 150 + 50 * n0y2;

	double x3 = 150;
	double y3 = 150;
	double n0x3 = cos(100);
	double n0y3 = sin(100);
	double L0x3 = 150 + 50 * n0x3;
	double L0y3 = 150 + 50 * n0y3;

	double x4 = 150;
	double y4 = 150;
	double n0x4 = cos(110);
	double n0y4 = sin(110);
	double L0x4 = 150 + 50 * n0x4;
	double L0y4 = 150 + 50 * n0y4;

	return(n0x1 * x + n0y1 * y - (L0x1*n0x1 + L0y1 * n0y1) <= 0 && n0x2 * x + n0y2 * y - (L0x2*n0x2 + L0y2 * n0y2) <= 0 && n0x3 * x + n0y3 * y - (L0x3*n0x3 + L0y3 * n0y3) <= 0 && n0x4 * x + n0y4 * y - (L0x4*n0x4 + L0y4 * n0y4) <= 0);
}

void setPixels()
{
	//set background color of image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			convex_poly[index++] = 255;
			convex_poly[index++] = 255;
			convex_poly[index] = 0;
		}
	}

	

	//set pixel color based on return value of function "shape" using Random Jittering Antialiasing
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
				int  index = (j * width + i) * 3;
					int r = 0;
					int g = 0;
					int b = 0;
					float ri = rand()%1;
					float rj = rand()%1;
					for (float m = 0; m < 6; m++)
					{
						for (float n = 0; n < 6; n++)
						{
							if (shape((i + m/6 + ri/6),( j + n/6+rj/6)) == 0) {
								r += 255;
								g += 255;
							}
							else
							{
								r += 0;
								g += 0;
							}
						}
					}
					r /= 36;
					g /= 36;
					b /= 36;
					convex_poly[index++] = r;
					convex_poly[index++] = g;
					convex_poly[index] = b;

		}
	}
	}




// =============================================================================
// OpenGL Display and Mouse Processing Functions.
//
// You can read up on OpenGL and modify these functions, as well as the commands
// in main(), to perform more sophisticated display or GUI behavior. This code
// will service the bare minimum display needs for most assignments.
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, convex_poly);
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
	convex_poly = new unsigned char[300 * 300 * 3];

	setPixels();

	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Convex Polygon");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; 
}


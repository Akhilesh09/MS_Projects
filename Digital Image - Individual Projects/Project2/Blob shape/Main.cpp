#include <cstdlib>
#include <iostream>
#include <GL/glut.h>
#include<math.h>

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
int width, height;
unsigned char *blob_shape;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "blob_shape."
// Then, "glutDisplayFunc" below will use blob_shape to display the pixel colors.
// =============================================================================

//returns true if pixel lies within blob shape and false otherwise.
int shape(float x, float y)
{
	return (pow((x - 150), 2) + pow((y - 150), 2) - pow(100, 2) <= 0 || pow((x - 80), 2) + pow((y - 100), 2) - pow(70, 2) <= 0 ||
		pow((x - 140), 2) + pow((y - 100), 2) - pow(70, 2) <= 0 || pow((x - 200), 2) + pow((y - 200), 2) - pow(70, 2) <= 0);
}

void setPixels()
{
	
	//set background color of image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			blob_shape[index++] = 255;
			blob_shape[index++] = 0xFF; 
			blob_shape[index] = 0x00; 
		}
	}
	
	//set pixel color based on return value of function "shape" using Random Jittering Antialiasing
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int  index = (y * width + x) * 3;
			int r = 0;
			int g = 0;
			int b = 0;
			float ri = rand() % 1;
			float rj = rand() % 1;
			for (float m = 0; m < 6; m++)
			{
				for (float n = 0; n < 6; n++)
				{
					if (shape((x + m / 6 + ri / 6), (y + n / 6 + rj / 6)) == 0) {
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
			blob_shape[index++] = r;
			blob_shape[index++] = g;
			blob_shape[index] = b;
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, blob_shape);
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
	blob_shape = new unsigned char[width * height * 3];  //

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Blob Shape");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; 
}

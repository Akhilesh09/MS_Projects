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
unsigned char *function_shape;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "function_shape."
// Then, "glutDisplayFunc" below will use function_shape to display the pixel colors.
// =============================================================================

//returns true if pixel lies within parabola shapes and false otherwise.
int shape(float x, float y)
{
	return (y - pow((x - 50), 2) - 200 <= 0 && y - pow((x - 75), 2) - 200 <= 0 && y - pow((x - 100), 2) - 200 <= 0 && y - pow((x - 125), 2) - 200 <= 0 && y - pow((x - 150), 2) - 200 <= 0 && y - pow((x - 175), 2) - 200 <= 0 && y - pow((x - 200), 2) - 200 <= 0 && y - pow((x - 225), 2) - 200 <= 0 && y - pow((x - 250), 2) - 200 <= 0);
}

void setPixels()
{
	
	//set background color of image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			function_shape[index++] = 255;
			function_shape[index++] = 0xFF;
			function_shape[index] = 0x00;
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
						r += 255;
						b += 255;
					}
				}
			}
			r /= 36;
			g /= 36;
			b /= 36;
			function_shape[index++] = r;
			function_shape[index++] = g;
			function_shape[index] = b;
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, function_shape);
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
	function_shape = new unsigned char[width * height * 3];  //

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Function Shape");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; 
}


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
unsigned char *shaded_sphere;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "shaded_sphere."
// Then, "glutDisplayFunc" below will use shaded_sphere to display the pixel colors.
// =============================================================================


//returns true if pixel lies within circle and false otherwise.
int shape1(float x, float y)
{
	return pow((x - 100), 2) + pow((y - 150), 2) - pow(80, 2) ;
}

//Euclidean Distance Function
float dist(float x1, float y1,float x2,float y2) {
	return sqrt(pow((x2-x1),2)+ pow((y2 - y1), 2));
}

void setPixels()
{
	
	//set pixel color based on distance from center using Random Jittering Antialiasing
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int  k = (y * width + x) * 3;
					int r = 0;
					int g = 0;
					int b = 0;
					float ri = rand() % 1;
					float rj = rand() % 1;
					for (float m = 0; m <6; m++)
					{
						for (float n = 0; n < 6; n++)
						{
							if (shape1((x + m / 6 + ri / 6), (y + n / 6 + rj / 6)) <= 0) {
								float a = dist((x + m / 6 + ri / 6), (y + n / 6 + rj / 6), 100, 150);
								r += 255 * (1 - a / 80.0);
								g += 255 * (1 - a / 80.0);
								b += 255 * (1 - a / 80.0);
							}
							else
							{
								r += 255;
								g += 0;
								b += 0;
							}
						}
					}
					r /= 36;
					g /= 36;
					b /= 36;
					shaded_sphere[k++] = r;
					shaded_sphere[k++] = g;
					shaded_sphere[k] = b;

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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, shaded_sphere);
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
	shaded_sphere = new unsigned char[width * height * 3];  //

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Shaded Sphere");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

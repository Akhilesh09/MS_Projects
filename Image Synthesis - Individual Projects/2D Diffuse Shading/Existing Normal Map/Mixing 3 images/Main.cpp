#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <string>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
//int width, height;
unsigned char *dark_arr;
unsigned char *dark_img;

unsigned char *light_arr;
unsigned char *light_img;

unsigned char *normal_arr;
unsigned char *normal_map;

unsigned char *spec_arr;
unsigned char *spec_img;

double *normal_mod;
double* dark_arr_mod;
double* light_arr_mod;
double* spec_arr_mod;
unsigned char *result;

int width = 300, height = 300, channels1, channels2, channels3;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================

double crop(double min, double max, double x) {
	x = (x - min) / (max - min);
	if (x > 1)
		x = 1;
	if (x < 0)
		x = 0;
	return x;
}
void setPixels()
{

	stbi_set_flip_vertically_on_load(true);
	dark_img = stbi_load("DI0b.png", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			dark_arr[i] = dark_img[i++];
			dark_arr[i] = dark_img[i++];
			dark_arr[i] = dark_img[i++];
		}
	}

	light_img = stbi_load("DI1.png", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			light_arr[i] = light_img[i++];
			light_arr[i] = light_img[i++];
			light_arr[i] = light_img[i++];
		}
	}

	spec_img = stbi_load("DI1b.png", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			spec_arr[i] = spec_img[i++];
			spec_arr[i] = spec_img[i++];
			spec_arr[i] = spec_img[i++];
		}
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			dark_arr_mod[i] = (double)(dark_arr[i] / 255.0);
			i++;
			dark_arr_mod[i] = (double)(dark_arr[i] / 255.0);
			i++;
			dark_arr_mod[i] = (double)(dark_arr[i] / 255.0);
			i++;
		}
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			light_arr_mod[i] = (double)(light_arr[i] / 255.0 );
			i++;
			light_arr_mod[i] = (double)(light_arr[i] / 255.0);
			i++;
			light_arr_mod[i] = (double)(light_arr[i] / 255.0);
			i++;
		}
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			spec_arr_mod[i] = (double)(spec_arr[i] / 255.0);
			i++;
			spec_arr_mod[i] = (double)(spec_arr[i] / 255.0);
			i++;
			spec_arr_mod[i] = (double)(spec_arr[i] / 255.0);
			i++;
		}
	}
	normal_map = stbi_load("SM.png", &width, &height, &channels3, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_arr[i] = normal_map[i++];
			normal_arr[i] = normal_map[i++];
			normal_arr[i] = normal_map[i++];
		}
	}


	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_mod[i] = (double)(2 * normal_arr[i] / 255.0 - 1);
			i++;
			normal_mod[i] = (double)(2 * normal_arr[i] / 255.0 - 1);
			i++;
			normal_mod[i] = (double)(2 * normal_arr[i] / 255.0 - 1);
			i++;
		}
	}
	
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;

				//normal_mod is normal map with values from -1 to 1
				//i is the x component, i+1 is the y component and i+2 is the z component
				// Assumed light at (1,1,0)
				double T = 0.5*(normal_mod[i] + normal_mod[i+1]) + 0.5;
				double S = 2 * normal_mod[i + 2] * (normal_mod[i] + normal_mod[i + 1]);
				double B = 1 - normal_mod[i + 2];

				T = crop(0, 1, T);
				S = crop(0, 1, S);
				
				if ((int)normal_arr[i + 2] != 0) {
					result[i] =  (light_arr[i] * (1-T) + dark_arr[i] * (T));
					result[i] = (result[i] * (1 -S) + spec_arr[i] * S);
				}
				else
					result[i] = (light_arr[i] * dark_arr[i] )/255;
					i++;

					if ((int)normal_arr[i + 1] != 0) {
						result[i] =  (light_arr[i] * (1-T)+dark_arr[i] * (T));
						result[i] = (result[i] * (1 -S) + spec_arr[i] * S);
					}
					else
						result[i] =  (light_arr[i] * dark_arr[i])/255;
					i++;

					if ((int)normal_arr[i] != 0) {
						result[i] =  (light_arr[i] * (1-T)+dark_arr[i] * (T));
						result[i] = (result[i] * (1 - S)+ spec_arr[i] * S);
					}
					else
						result[i] =(light_arr[i] * dark_arr[i])/255;
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, result);
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
	dark_arr = new unsigned char[300 * 300 * 3];
	light_arr = new unsigned char[300 * 300 * 3];
	normal_arr = new unsigned char[300 * 300 * 3];
	spec_arr = new unsigned char[300 * 300 * 3];
	spec_arr_mod = new double[300 * 300 * 3];
	normal_mod = new double[300 * 300 * 3];
	dark_arr_mod = new double[300 * 300 * 3];
	light_arr_mod = new double[300 * 300 * 3];
	result = new unsigned char[300 * 300 * 3];

	setPixels();


	// OpenGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefiniteL_y to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Homework Zero");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; //This line never gets reached. We use it because "main" is T_ype int.
}

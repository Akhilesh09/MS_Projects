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
unsigned char *env_img;
unsigned char *depth_img;

unsigned char *depth_arr;
unsigned char *env_arr;
unsigned char *result;

unsigned char *dark_arr;
unsigned char *dark_img;

unsigned char *light_arr;
unsigned char *light_img;

unsigned char *normal_arr;
unsigned char *normal_map;

unsigned char *spec_arr;
unsigned char *spec_img;

float *normal_mod;
float* dark_arr_mod;
float* light_arr_mod;
float* spec_arr_mod;

int width = 300, height = 300, channels1, channels2, channels3;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================



float crop(float min, float max, float x) {
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
	dark_img = stbi_load("sphere_d.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			dark_arr[i] = dark_img[i++];
			dark_arr[i] = dark_img[i++];
			dark_arr[i] = dark_img[i++];
		}
	}

	light_img = stbi_load("sphere_l.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			light_arr[i] = light_img[i++];
			light_arr[i] = light_img[i++];
			light_arr[i] = light_img[i++];
		}
	}

	spec_img = stbi_load("sphere_s.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			spec_arr[i] = spec_img[i++];
			spec_arr[i] = spec_img[i++];
			spec_arr[i] = spec_img[i++];
		}
	}

	depth_img = stbi_load("NormalMap2.png", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_arr[i] = depth_img[i++];
			normal_arr[i] = depth_img[i++];
			normal_arr[i] = depth_img[i++];
		}
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			normal_mod[i] = (float)(2*(int)normal_arr[i] / 255.0-1);
			i++;
			normal_mod[i] = (float)(2 * (int)normal_arr[i] / 255.0 - 1);
			i++;
			normal_mod[i] = (float)(2 * (int)normal_arr[i] / 255.0 - 1);
			i++;
		}
	}

	env_img = stbi_load("back.png", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			env_arr[i] = env_img[i++];
			env_arr[i] = env_img[i++];
			env_arr[i] = env_img[i++];
		}
	}

	int d =14;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			int indx = abs(((2 * normal_mod[i] * normal_mod[i + 2] * d) / (-1 + 2 * normal_mod[i + 2] * normal_mod[i + 2])) + x);
			int indy = abs(((2 * normal_mod[i + 1] * normal_mod[i + 2] * d) / (-1 + 2 * normal_mod[i + 2] * normal_mod[i + 2])) + y);
			indx = indx % 300;
			indy = indy % 300;
			int ind = (indy*width + indx) * 3;

			float T = 0.5*(normal_mod[i] + normal_mod[i + 1]) + 0.5;
			float S = 2 * normal_mod[i + 2] * (normal_mod[i] + normal_mod[i + 1]);
			float B = 1 - normal_mod[i + 2];

			float ks = 0.1;
			float kr = 1;

			T = crop(0, 1, T);
			S = crop(0, 1, S);

			result[i] = (light_arr[i] * (1 - T) + dark_arr[i] * (T));
			result[i] = (1 - (kr))*result[i] + kr * env_arr[ind++];
			i++;

			result[i] = (light_arr[i] * (1 - T) + dark_arr[i] * (T));
			result[i] = (1 - (kr))*result[i] + kr * env_arr[ind++];
			i++;

			result[i] = (light_arr[i] * (1 - T) + dark_arr[i] * (T));
			result[i] = (1 - (kr))*result[i] + kr * env_arr[ind++];
			i++;
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
	spec_arr = new unsigned char[300 * 300 * 3];
	depth_arr = new unsigned char[300 * 300 * 3];
	env_arr = new unsigned char[300 * 300 * 3];
	normal_arr = new unsigned char[300 * 300 * 3];
	normal_mod = new float[300 * 300 * 3];
	dark_arr_mod = new float[300 * 300 * 3];
	light_arr_mod = new float[300 * 300 * 3];
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

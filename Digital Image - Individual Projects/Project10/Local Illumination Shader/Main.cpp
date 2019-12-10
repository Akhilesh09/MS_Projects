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

unsigned char *dark_image;
unsigned char *image1;

unsigned char *light_image;
unsigned char *image2;

unsigned char *normal_map;
unsigned char *image3;

double *normal_map_mod;
unsigned char *out_img;

int width = 300, height = 300, channels1, channels2, channels3;

void setPixels()
{

	//initilaize array with dark image
	stbi_set_flip_verticallight_y_on_load(false);
	image1 = stbi_load("dark.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int dark_ind = (y * width + x) * 3;
			dark_image[dark_ind] = image1[dark_ind++];
			dark_image[dark_ind] = image1[dark_ind++];
			dark_image[dark_ind] = image1[dark_ind++];
		}
	}

	//initilaize array with light image
	image2 = stbi_load("light.jpg", &width, &height, &channels2, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int light_ind = (y * width + x) * 3;
			light_image[light_ind] = image2[light_ind++];
			light_image[light_ind] = image2[light_ind++];
			light_image[light_ind] = image2[light_ind++];
		}
	}

	//initilaize array with normal map
	image3 = stbi_load("normal.jpg", &width, &height, &channels3, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int normal_ind = (y * width + x) * 3;
			normal_map[normal_ind] = image3[normal_ind++];
			normal_map[normal_ind] = image3[normal_ind++];
			normal_map[normal_ind] = image3[normal_ind++];
		}
	}

	//initilaize array with normal map values in -1 to 1
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int normal_mod_ind = (y * width + x) * 3;
			normal_map_mod[normal_mod_ind] = (double)(2 * normal_map[normal_mod_ind] / 255.0 - 1);
			normal_mod_ind++;
			normal_map_mod[normal_mod_ind] = (double)(2 * normal_map[normal_mod_ind] / 255.0 - 1);
			normal_mod_ind++;
			normal_map_mod[normal_mod_ind] = (double)(2 * normal_map[normal_mod_ind] / 255.0 - 1);
			normal_mod_ind++;
		}
	}

	//adjust brightness of pixels based on dot product with coordinates of light vector
	
	double light_x[100], light_y[100];


	for (int t = 0; t < 100; t++)
	{
		light_x[t] = sin(t * 5);
		light_y[t] = cos(t * 5);
	}

	for (int t = 0; t < 100; t++)
	{
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int out_ind = (y * width + x) * 3;
				double color_at_t_x = 0.5*(normal_map_mod[out_ind] * light_x[t]) + 0.5;
				out_img[out_ind] = dark_image[out_ind] * (1 - color_at_t_x) + light_image[out_ind] * color_at_t_x;
				out_ind++;
				double color_at_t_y = 0.5*(normal_map_mod[out_ind] * light_y[t]) + 0.5;
				out_img[out_ind] = dark_image[out_ind] * (1 - color_at_t_y) + light_image[out_ind] * color_at_t_y;
				out_ind++;
				double color_at_t_z = 0.5*(normal_map_mod[out_ind] * 0) + 0.5;
				out_img[out_ind] = dark_image[out_ind] * (1 - color_at_t_z) + light_image[out_ind] * color_at_t_z;
				out_ind++;
			}
		}

		string filename = "output/frame" + to_string(t) + ".jpg";
		stbi_write_jpg(filename.c_str(), 300, 300, channels1, out_img, 100);
	}
}

static void windowResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdenticolor_at_t_y();
	glOrtho(0, (w / 2), 0, (h / 2), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdenticolor_at_t_y();
}
static void windowDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, out_img);
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
	dark_image = new unsigned char[300 * 300 * 3];
	light_image = new unsigned char[300 * 300 * 3];
	normal_map = new unsigned char[300 * 300 * 3];
	normal_map_mod = new double[300 * 300 * 3];
	out_img = new unsigned char[300 * 300 * 3];

	setPixels();

	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Illumination Shader");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}
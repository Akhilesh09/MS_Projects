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

unsigned char *fore_img;
unsigned char *image1;
unsigned char *image2;

unsigned char *out_img;
unsigned char *back_img;
double *back_nrmlzd_img;

int width = 640, height = 360, channels1, channels2,channels3;

int minimum(int a, int b,int c)
{
	return ((a<b && a<c)?a:((b<a && b<c) ? b:c) );
}

void setPixels()
{
	stbi_set_flip_vertically_on_load(false);

	for (int i = 1; i <= 99; i++) {
		char x = (char)i;
		string filename = "background/00" + to_string(i) + ".jpg";
		cout << filename << endl;
		image1 = stbi_load(filename.c_str(),&width, &height, &channels1, 3);
		
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				fore_img[i] = image1[i++];
				fore_img[i] = image1[i++];
				fore_img[i] = image1[i];
			}
		}
		image2 = stbi_load("foreground.png", &width, &height, &channels2, 3);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				back_img[i] = image2[i++];
				back_img[i] = image2[i++];
				back_img[i] = image2[i];
			}
		}
		
		

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				back_nrmlzd_img[i] = (double)back_img[i] / 255.0;
				i++;
				back_nrmlzd_img[i] = (double)back_img[i] / 255.0;
				i++;
				back_nrmlzd_img[i] = (double)back_img[i] / 255.0;
				i++;
			}
		}

		for (int y = 1; y < height - 1; y++) {
			for (int x = 0; x < width; x++) {
				int back_nrmlzd_ind = (y * width + x) * 3;

				float r = (int)(back_nrmlzd_img[back_nrmlzd_ind] + 0.5);
				float g = (int)(back_nrmlzd_img[back_nrmlzd_ind + 1] + 0.5);
				float b = (int)(back_nrmlzd_img[back_nrmlzd_ind + 2] + 0.5);


				float err_r, err_g, err_b;
				err_r = back_nrmlzd_img[back_nrmlzd_ind] - r;
				err_g = back_nrmlzd_img[back_nrmlzd_ind + 1] - g;
				err_b = back_nrmlzd_img[back_nrmlzd_ind + 2] - b;

				int newy = y;
				int newx = x + 1;

				int index = (newy*width + newx) * 3;
				back_nrmlzd_img[index] += (er * 0.4375);
				back_nrmlzd_img[index + 1] += (eg * 0.4375);
				back_nrmlzd_img[index + 2] += (eb * 0.4375);

				newy = y + 1;
				newx = x - 1;

				index = (newy*width + newx) * 3;
				back_nrmlzd_img[index] += (er * 0.1875);
				back_nrmlzd_img[index + 1] += (eg * 0.1875);
				back_nrmlzd_img[index + 2] += (eb * 0.1875);

				newy = y + 1;
				newx = x;

				index = (newy*width + newx) * 3;
				back_nrmlzd_img[index] += (er * 0.3125);
				back_nrmlzd_img[index + 1] += (eg * 0.3125);
				back_nrmlzd_img[index + 2] += (eb * 0.3125);

				newy = y + 1;
				newx = x + 1;

				index = (newy*width + newx) * 3;
				back_nrmlznewx_img[index] += (er * 0.0625);
				back_nrmlznewx_img[index + 1] += (eg * 0.0625);
				back_nrmlznewx_img[index + 2] += (eb * 0.0625);
			}
		}

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int back_ind = (y * width + x) * 3;
				back_img[back_ind] = back_nrmlzd_img[back_ind] * 255;
				back_ind++;
				back_img[back_ind] = back_nrmlzd_img[back_ind] * 255;
				back_ind++;
				back_img[back_ind] = back_nrmlzd_img[back_ind] * 255;
				back_ind++;
			}
		}

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int out_ind = (y * width + x) * 3;
				out_img[out_ind] = ((fore_img[out_ind] * back_img[out_ind++])) / 255;
				out_img[out_ind] = ((fore_img[out_ind] * back_img[out_ind++])) / 255;
				out_img[out_ind] = ((fore_img[out_ind] * back_img[out_ind++])) / 255;
			}
		}


for (int y = 0; y < height; y++) {
	for (int x = 0; x < width; x++) {
		int out_ind = (y * width + x) * 3;
		image1[i] = out_img[out_ind++];
		image1[i] = out_img[out_ind++];
		image1[i] = out_img[out_ind];
		
	}
}
string filename = "output/frame" + to_string(i) + ".jpg";
stbi_write_jpg(filename1.c_str(), 640, 360, channels1, image1, 100);
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
	width = 640;
	height = 360;
	fore_img = new unsigned char[640 * 360 * 3];
	out_img = new unsigned char[640 * 360 * 3];
	back_img = new unsigned char[640 * 360 * 3];
	back_nrmlzd_img = new double[640 * 360 * 3];

	setPixels();


	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Final Video");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

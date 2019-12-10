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

unsigned char *org_img;
unsigned char *inp_img;

unsigned char *output_img;
double *emboss_x;
double *emboss_y;
double filter_x_arr[9] = {-1,-1,-1,0,0,0,1,1,1};
double filter_y_arr[9] = { 1,0,-1,1,0,-1,1,0,-1 };
int width = 300, height = 168, channels1, channels2;

void new_val(int x, int y,int n)
{
	double q=0, p=0, o=0;
	int m = 1;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			org_img[index++] = q;
			org_img[index++] = p;
			org_img[index] = o;
		}
	}
}

void setPixels()
{
	stbi_set_flip_vertically_on_load(true);
	inp_img = stbi_load("forest.jpg", &width, &height, &channels1, STBI_rgb);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int org_ind = (y * width + x) * 3;
			org_img[org_ind++] = inp_img[org_ind++];
			org_img[org_ind++] = inp_img[org_ind++];
			org_img[org_ind] = inp_img[org_ind];
		}
	}

	int filter_x_ind = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			int emb_x_ind = (y * 3 + x)*3 ;
			emboss_x[emb_x_ind++] = c[filter_x_ind];
			emboss_x[emb_x_ind++] = c[filter_x_ind];
			emboss_x[emb_x_ind] = c[filter_x_ind];
			filter_x_ind = filter_x_ind + 1;
		}
	}

	int filter_y_ind = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			int emb_y_ind = (y * 3 + x) * 3;
			emboss_y[emb_y_ind++] = d[filter_y_ind];
			emboss_y[emb_y_ind++] = d[filter_y_ind];
			emboss_y[emb_y_ind] = d[filter_y_ind];
			filter_y_ind = filter_y_ind + 1;
		}
	}

	double q, p, o;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			q = 0, p = 0, o = 0;
			for (int i = -1; i <= +1; i++) {
				for (int j = -1; j <= +1; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newy > height - 1)
						newy = (height) - (newy % (height)) - 1;
					else if (newy < 0)
						newy = (height) - (newy % (height)) - 1;
					if (newx > width-1)
						newx = (width) - (newx % (width)) - 1;
					else if (newx < 0)
						newx = (width) - (newx % (width)) - 1;
					int org_ind = ((i + 1) * 3 + (j + 1)) * 3;
					int  emb_x_ind= (newy * width + newx) * 3;
					q += org_img[org_ind++] * emboss_x[emb_x_ind++];
					p += org_img[org_ind++] * emboss_x[emb_x_ind++];
					o += org_img[org_ind] * emboss_x[emb_x_ind];
				}
			} 
		}
	}
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			q = 0, p = 0, o = 0;
			for (int i = -1; i <= +1; i++) {
				for (int j = -1; j <= +1; j++) {
					int newy = y + i;
					int newx = x + j;
					if (newy > height - 1)
						newy = (height - 1) - (newy % (height - 1)) - 1;
					else if (newy < 0)
						newy = (height - 1) - (newy % (height - 1)) - 1;
					if (newx > width - 1)
						newx = (width - 1) - (newx % (width - 1)) - 1;
					else if (newx < 0)
						newx = (width - 1) - (newx % (width - 1)) - 1;
					int org_ind = ((i + 1) * 3 + (j + 1)) * 3;
					int emb_y_ind = (newy * width + newx) * 3;
					q += org_img[org_ind++] * emboss_y[emb_y_ind++];
					p += org_img[org_ind++] * emboss_y[emb_y_ind++];
					o += org_img[org_ind] * emboss_y[emb_y_ind];
				}
			}
			int out_ind = (y * width + x) * 3;
			output_img[out_ind++] = (q+765)/6;
			output_img[out_ind++] = (p + 765) / 6;
			output_img[out_ind] = (o + 765) / 6;
		}
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
	height = 168;
	org_img = new unsigned char[300 * 300 * 3];
	output_img = new unsigned char[300 * 300 * 3];
	emboss_x = new double[3 * 3*3];
	emboss_y = new double[3 * 3 * 3];



	setPixels();

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Emboss Filter");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}

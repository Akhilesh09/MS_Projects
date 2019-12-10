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

unsigned char *inp_img1;
unsigned char *image1;
unsigned char *image2;

unsigned char *inp_img2;
unsigned char *out_img;

int width = 300, height = 300, channels1, channels2,channels3;

int minimum(int a, int b,int c)
{
	return ((a<b && a<c)?a:((b<a && b<c) ? b:c) );
}

void setPixels()
{
	
	//initilaize array with input image1
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("forest1.jpg", &width, &height, &channels1, 3);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int inp1_ind = (y * width + x) * 3;
			inp_img1[inp1_ind] = image1[inp1_ind++];
			inp_img1[inp1_ind] = image1[inp1_ind++];
			inp_img1[inp1_ind] = image1[inp1_ind];
		}
	}
	
	//initilaize array with input image2
	image2 = stbi_load("forest3.jpg", &width, &height, &channels2, 3);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int inp2_ind = (y * width + x) * 3;
			inp_img2[inp2_ind] = image2[inp2_ind++];
			inp_img2[inp2_ind] = image2[inp2_ind++];
			inp_img2[inp2_ind] = image2[inp2_ind];
		}
	}
	
	//find minimum energy seam

	int count = 0;
	while (count < 80)
	{

	//find minimum energy seam
	
		for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int inp2_ind = (y * width + x) * 3;
			inp_img2[inp2_ind] = 255;
			inp_img2[inp2_ind] = 255;
			inp_img2[inp2_ind] = 255;


		int min_seam[300][300];

		for (int y = 0; y < 300; y++) {
			for (int x = 0; x < 300; x++) {
				min_seam[x][y] = 0;
			}
		}

		int x, y, total, parent, newx, newy;
		for (x = 0; x < width - 1; x++) {
			total = 0;
			parent = ((int)inp1_img[width * 3] + (int)inp1_img[width * 3 + 1] + (int)inp1_img[width * 3 + 2]) / 3;
			newx = 1;
			newy = 0;
		for (y = 1; y < height - count - 1; y++) {
			
				int child1 = ((int)inp1_img[((y+1)*width + (x - 1)) * 3] + (int)inp1_img[((y+1)*width + (x - 1) + 1) * 3] + (int)inp1_img[((y+1 )*width + (x - 1) + 2) * 3]) / 3;
				int child2 = ((int)inp1_img[((y+1)*width + (x)) * 3] + (int)inp1_img[((y+1)*width + (x )) * 3 + 1] + (int)inp1_img[((y+1)*width + (x)) * 3 + 2]) / 3;
				int child3 = ((int)inp1_img[((y+1)*width + (x + 1)) * 3] + (int)inp1_img[((y+1)*width + (x + 1)) * 3 + 1] + (int)inp1_img[((y+1)*width + (x + 1)) * 3 + 2]) / 3;
				int min1 = abs(child1 - parent);
				int min2 = abs(child2 - parent);
				int min3 = abs(child3 - parent);
				int min = minimum(min1, min2, min3);
				if (min == min1)
				{
					min_seam[x][y] = ((y+1 )*width + (x - 1)) * 3;
					parent = child1;
					newx = x - 1;
					newy = y+1 ;
				}
				else if (min == min2)
				{
					min_seam[x][y] = ((y+1)* width + (x)) * 3;
					parent = child2;
					newx = x ;
					newy = y+1;
				}
				else
				{
					min_seam[x][y] = ((y+1 )*width + (x + 1)) * 3;
					parent = child3;
					newx = x + 1;
					newy = y+1 ;
				}
				total += min;
			}
			min_seam[x][299] = total;
		}
		int min_indx = 0;
		int min_indy = 299;
		int minimum = min_seam[min_indx][min_indy];

		for (int x=1; x < width - count - 1; x++)
		{
			if (min_seam[x][299] < minimum)
			{
				min_indx = x;
				minimum = min_seam[x][299];
			}
		}

		for (int y = 0; y < 299; y++) {
			inp1_img[min_seam[min_indx][y]] = 0;
			inp1_img[min_seam[min_indx][y] + 1] = 0;
			inp1_img[min_seam[min_indx][y] + 2] = 0;
		}
		int i;
		for (int y = 0; y < height; y++) {
			int seam_indx = 0, flag = 0;
		for (int x = 0; x < width-count; x++) {
				inp2_ind = (y * width + x) * 3;
				if (inp1_img[i] == 0 && inp1_img[i + 1] == 0 && inp1_img[i + 2] == 0)
				{
					seam_indx = x;
					flag = 1;
					break;
				}
				else
				{
					inp2_img[inp2_ind] = inp1_img[inp2_ind++];
					inp2_img[inp2_ind] = inp1_img[inp2_ind++];
					inp2_img[inp2_ind] = inp1_img[inp2_ind];
				}
			}

			if (flag == 1) {
				int k;
				for (k = seam_indx; k < width-count; k++) {
					int inp2_ind = (y * width + k) * 3;
					int inp1_ind = ((y) * width + k+1) * 3;
					inp2_img[inp2_ind] = inp1_img[inp1_ind];
					inp2_img[inp2_ind + 1] = inp1_img[inp1_ind + 1];
					inp2_img[inp2_ind + 2] = inp1_img[inp1_ind + 2];
				}
			}
		}
	
	//remove minimum energy seam
	
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int inp1_ind = (y * width + x) * 3;

					inp1_img[inp1_ind] = 255;
					inp1_img[inp1_ind++] = 255;
					inp1_img[inp1_ind++] = 255;
				}
			}

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int inp1_ind = (y * width + x) * 3;

					inp1_img[inp1_ind] = inp2_img[inp1_ind++];
					inp1_img[inp1_ind] = inp2_img[inp1_ind++];
					inp1_img[inp1_ind] = inp2_img[inp1_ind];
				}
			}
			count++;
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, inp1_img);
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
	inp1_img = new unsigned char[300 * 300 * 3];
	inp2_img = new unsigned char[300 * 300 * 3];

	setPixels();

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

	return 0;
}

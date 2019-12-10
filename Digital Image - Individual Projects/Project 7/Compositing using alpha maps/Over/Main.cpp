// =============================================================================
// VIZA654/CSCE646 at Texas A&M University
// Homework 0
// Created by Anton Agana based from Ariel Chisholm's template
// 05.23.2011
//
// This file is supplied with an associated makefile. Put both files in the same
// directory, navigate to that directory from the Linux shell, and type 'make'.
// This will create a program called 'pr01' that you can run by entering
// 'homework0' as a command in the shell.
//
// If you are new to programming in Linux, there is an
// excellent introduction to makefile structure and the gcc compiler here:
//
// http://www.cs.txstate.edu/labs/tutorials/tut_docs/Linux_Prog_Environment.pdf
//
// =============================================================================

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

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
//int width, height;
unsigned char *fore_img;
unsigned char *image1;

unsigned char *back_img;
unsigned char *image2;

unsigned char *out_img;
int width = 300, height = 300, channels1, channels2;


// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================
int maximum(int a, int b)
{
	return a > b ? a : b;
}

void setPixels()
{
	stbi_set_flip_vertically_on_load(true);
	image1 = stbi_load("fore.png", &width, &height, &channels1, 4);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int fore_ind = (y * width + x) * 4;
			fore_img[fore_ind] = image1[fore_ind++];
			fore_img[fore_ind] = image1[fore_ind++];
			fore_img[fore_ind] = image1[fore_ind++];
			fore_img[fore_ind] = image1[fore_ind];
		}
	}
	image2 = stbi_load("back.png", &width, &height, &channels2, 4);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int back_ind = (y * width + x) * 4;
			back_img[back_ind] = image2[back_ind++];
			back_img[back_ind] = image2[back_ind++];
			back_img[back_ind] = image2[back_ind++];
			back_img[back_ind] = image2[back_ind];
		}
	}
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int out_ind = (y * width + x) * 4;
			out_img[out_ind] = (back_img[out_ind]*0.35+fore_img[out_ind]*0.65);
			out_ind++;
			out_img[out_ind] = (back_img[out_ind] * 0.35 + fore_img[out_ind] * 0.65);
			out_ind++;
			out_img[out_ind] = (back_img[out_ind] * 0.35 + fore_img[out_ind] * 0.65);
			out_ind++;
			out_img[out_ind] = (back_img[out_ind] * 0.35 + fore_img[out_ind] * 0.65);
			out_ind++;
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
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, out_img);
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
	fore_img = new unsigned char[300 * 300 * 4];
	back_img = new unsigned char[300 * 300 * 4];
	out_img = new unsigned char[300 * 300 * 4];


	setPixels();


	// OpenGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefinitely to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Over Operation");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; //This line never gets reached. We use it because "main" is type int.
}

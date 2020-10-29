#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <string>
#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
//int width, height;
unsigned char* texture_img[9];

unsigned char* result;
unsigned char* BI_res;

int width = 256, height = 256, channels1;
int width1[9], height1[9];

void BI(float u, float v, int d)
{
	float u_p = u * (width1[d] - 1);
	float v_p = v * (height1[d] - 1);
	float Ax = 0, Ay = 0, Bx = 0, By = 0, Cx = 0, Cy = 0, Dx = 0, Dy = 0;

	for (float m = 0; m < width1[d]; m++)
	{
		if (m > u_p)
		{
			Ax = m - 1;
			Cx = Ax;
			Bx = m;
			Dx = Bx;
			break;
		}
	}
	for (float n = 0; n < height1[d]; n++)
	{
		if (n > v_p)
		{
			Ay = n - 1;
			By = Ay;
			Cy = n;
			Dy = Cy;
			break;
		}
	}

	//interpolate between AB and CD...then interpolate between those 2 points
	BI_res[0] = (Cy - v_p) * (texture_img[d][int(Ay * width1[d] + Ax) * 3] * (Bx - u_p) + texture_img[d][int(By * width1[d] + Bx) * 3] * (u_p - Ax)) + (v_p - Ay) * (texture_img[d][int(Cy * width1[d] + Cx) * 3] * (Bx - u_p) + texture_img[d][int(Dy * width1[d] + Dx) * 3] * (u_p - Ax));
	BI_res[1] = (Cy - v_p) * (texture_img[d][int(Ay * width1[d] + Ax) * 3 + 1] * (Bx - u_p) + texture_img[d][int(By * width1[d] + Bx) * 3 + 1] * (u_p - Ax)) + (v_p - Ay) * (texture_img[d][int(Cy * width1[d] + Cx) * 3 + 1] * (Bx - u_p) + texture_img[d][int(Dy * width1[d] + Dx) * 3 + 1] * (u_p - Ax));
	BI_res[2] = (Cy - v_p) * (texture_img[d][int(Ay * width1[d] + Ax) * 3 + 2] * (Bx - u_p) + texture_img[d][int(By * width1[d] + Bx) * 3 + 2] * (u_p - Ax)) + (v_p - Ay) * (texture_img[d][int(Cy * width1[d] + Cx) * 3 + 2] * (Bx - u_p) + texture_img[d][int(Dy * width1[d] + Dx) * 3 + 2] * (u_p - Ax));

}


//euclidean distance between 2 points
float dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

//vector class and operations
class Vector
{
public:
	float x, y, z;
	friend Vector operator-(Vector a, Vector b)
	{
		Vector res;
		res.x = a.x - b.x;
		res.y = a.y - b.y;
		res.z = a.z - b.z;
		return res;
	}
	friend Vector operator+(Vector a, Vector b)
	{
		Vector res;
		res.x = a.x + b.x;
		res.y = a.y + b.y;
		res.z = a.z + b.z;
		return res;
	}
	friend float operator*(Vector a, Vector b)
	{

		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	friend Vector operator*(Vector a, float b)
	{
		Vector res;
		res.x = a.x * b;
		res.y = a.y * b;
		res.z = a.z * b;
		return res;
	}


};

// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================
float magnitude(float a, float b, float c)
{
	return sqrt(pow(a, 2) + pow(b, 2) + pow(c, 2));
}

Vector cross_product(Vector a, Vector b)
{
	Vector res;
	res.x = a.y * b.z - b.y * a.z;
	res.y = a.z * b.x - b.z * a.x;
	res.z = a.x * b.y - b.x * a.y;
	return res;
}

void setPixels(string filterType)
{
	stbi_set_flip_vertically_on_load(true);
	int size = 1;

	//load mipmaps
	for (int i = 0; i < 9; i++)
	{
		texture_img[i] = stbi_load((filterType + std::to_string(256 / size) + "by" + std::to_string(256 / size) + ".jpg").c_str(), &width1[i], &height1[i], &channels1, STBI_rgb);
		size *= 2;
	}

	//camera setup
	//camera up vector
	Vector Vup;
	Vup.x = 150;
	Vup.y = 15;
	Vup.z = -100;

	//camera view direction
	Vector V_view;
	V_view.x = 10;
	V_view.y = 10;
	V_view.z = 100;

	//camera local normals
	Vector V0 = cross_product(V_view, Vup);
	Vector n0;
	n0 = V0 * (1 / magnitude(V0.x, V0.y, V0.z));
	Vector n2;
	n2 = V_view * (1 / magnitude(V_view.x, V_view.y, V_view.z));

	Vector n1 = cross_product(n0, n2);

	//eyepoint
	Vector Pe;
	Pe.x = -100;
	Pe.y = 100;
	Pe.z = -200;

	//camera dimensions and distance from eyepoint
	float d = 140, sx = 150;
	float sy = sx * height / width;

	//center of camera
	Vector P_Cam;
	P_Cam = Pe + (n2 * d);

	//bottom-left corner of camera
	Vector P00;
	P00 = P_Cam - (n0 * (sx / 2)) - (n1 * (sy / 2));

	//point on plane
	Vector P_p1;
	P_p1.x = -100;
	P_p1.y = 100;
	P_p1.z = -80;
	Vector n_p1 = P_p1 * (1 / magnitude(P_p1.x, P_p1.y, P_p1.z));

	Vector n_v2;
	n_v2.x = 25;
	n_v2.y = -60;
	n_v2.z = 50;
	n_v2 = n_v2 * (1 / magnitude(n_v2.x, n_v2.y, n_v2.z));


	Vector n_p2 = cross_product(n_p1, n_v2);

	Vector n_p3 = cross_product(n_p1, n_p2);

	//dimensions for repeating texture
	float p_sx = 30, p_sy = p_sx * height / width;

	//loop through image with infinite plane
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int i = (y * width + x) * 3;

			Vector Pp, npe, P_hit;

			//map pixel to point on camera plane
			Pp = P00 + (n0 * (sx * (x + 0.5) / width)) + (n1 * (sy * (y + 0.5) / height));

			//ray direction
			npe = Pp - Pe;
			npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

			float num = (n_p1 * (P_p1 - Pe));
			float den = (n_p1 * npe);

			//check if ray intersects pixel
			if (num < 0 && den < 0)
			{
				//find hitpoint
				float t_hit = num / den;
				P_hit = Pe + (npe * t_hit);

				//find u,v of pixel center
				float u = (n_p2 * (P_hit - P_p1)) / p_sx;
				float v = (n_p3 * (P_hit - P_p1)) / p_sy;

				if (u < 0)
				{
					u = u - floor(u);
					u = 1 - u;
				}
				else
				{
					u = u - floor(u);
				}
				if (v < 0)
				{
					v = v - floor(v);
					v = 1 - v;
				}
				else
				{
					v = v - floor(v);
				}

				Pp = P00 + (n0 * (sx * (x) / width)) + (n1 * (sy * (y) / height));

				npe = Pp - Pe;
				npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

				num = (n_p1 * (P_p1 - Pe));
				den = (n_p1 * npe);

				t_hit = num / den;
				P_hit = Pe + (npe * t_hit);

				//find u,v of pixel bottom-left corner
				float u1 = (n_p2 * (P_hit - P_p1)) / p_sx;
				float v1 = (n_p3 * (P_hit - P_p1)) / p_sy;


				if (u1 < 0)
				{
					u1 = u1 - floor(u1);
					u1 = 1 - u1;
				}
				else
				{
					u1 = u1 - floor(u1);
				}
				if (v1 < 0)
				{
					v1 = v1 - floor(v1);
					v1 = 1 - v1;
				}
				else
				{
					v1 = v1 - floor(v1);
				}

				//find u,v of pixel bottom-right corner
				Pp = P00 + (n0 * (sx * (x + 1) / width)) + (n1 * (sy * (y) / height));

				npe = Pp - Pe;
				npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

				num = (n_p1 * (P_p1 - Pe));
				den = (n_p1 * npe);

				t_hit = num / den;
				P_hit = Pe + (npe * t_hit);

				float u2 = (n_p2 * (P_hit - P_p1)) / p_sx;
				float v2 = (n_p3 * (P_hit - P_p1)) / p_sy;


				if (u2 < 0)
				{
					u2 = u2 - floor(u2);
					u2 = 1 - u2;
				}
				else
				{
					u2 = u2 - floor(u2);
				}
				if (v2 < 0)
				{
					v2 = v2 - floor(v2);
					v2 = 1 - v2;
				}
				else
				{
					v2 = v2 - floor(v2);
				}


				Pp = P00 + (n0 * (sx * (x) / width)) + (n1 * (sy * (y + 1) / height));

				npe = Pp - Pe;
				npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

				num = (n_p1 * (P_p1 - Pe));
				den = (n_p1 * npe);

				t_hit = num / den;
				P_hit = Pe + (npe * t_hit);
				//find u,v of pixel top-left corner
				float u3 = (n_p2 * (P_hit - P_p1)) / p_sx;
				float v3 = (n_p3 * (P_hit - P_p1)) / p_sy;


				if (u3 < 0)
				{
					u3 = u3 - floor(u3);
					u3 = 1 - u3;
				}
				else
				{
					u3 = u3 - floor(u3);
				}
				if (v3 < 0)
				{
					v3 = v3 - floor(v3);
					v3 = 1 - v3;
				}
				else
				{
					v3 = v3 - floor(v3);
				}


				Pp = P00 + (n0 * (sx * (x + 1) / width)) + (n1 * (sy * (y + 1) / height));

				npe = Pp - Pe;
				npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

				num = (n_p1 * (P_p1 - Pe));
				den = (n_p1 * npe);

				t_hit = num / den;
				P_hit = Pe + (npe * t_hit);
				//find u,v of pixel top-right corner
				float u4 = (n_p2 * (P_hit - P_p1)) / p_sx;
				float v4 = (n_p3 * (P_hit - P_p1)) / p_sy;


				if (u4 < 0)
				{
					u4 = u4 - floor(u4);
					u4 = 1 - u4;
				}
				else
				{
					u4 = u4 - floor(u4);
				}
				if (v4 < 0)
				{
					v4 = v4 - floor(v4);
					v4 = 1 - v4;
				}
				else
				{
					v4 = v4 - floor(v4);
				}


				//find the length of axes
				float v_axis = dist((u2 + u1) / 2, (v2 + v1) / 2, (u3 + u4) / 2, (v3 + v4) / 2);
				float u_axis = dist((u3 + u1) / 2, (v3 + v1) / 2, (u4 + u2) / 2, (v4 + v2) / 2);

				float ratio;
				float shorter;

				//find ratio of longer axis/shorter axis and store shorter axis
				if (u_axis > v_axis)
				{
					ratio = u_axis / v_axis;
					shorter = v_axis;
				}
				else
				{
					ratio = v_axis / u_axis;
					shorter = u_axis;
				}

				//find number of samples along longer axis
				int num_samples = ceil(ratio);

				//capping max samples
				if (num_samples > 20)
					num_samples = 20;

				//find mipmap level d
				int d = 8 + log(shorter);

				if (d < 0)
					d = 8;

				if (shorter == u_axis)
				{
					//average along v
					float v_increments = ((v3 + v4) / 2 - (v1 + v2) / 2) / num_samples;
					float h_increments = ((u3 + u4) / 2 - (u1 + u2) / 2) / num_samples;
					double r = 0, g = 0, b = 0, start_h = (u1 + u2) / 2 + 0.5 * h_increments, start_v = (v1 + v2) / 2 + 0.5 * h_increments;

					for (float s = 0; s < num_samples; s++)
					{
						//bilinearly interpolate in the same level
						BI(start_h, start_v, d);

						r += BI_res[0];
						g += BI_res[1];
						b += BI_res[2];

						start_h += h_increments;
						start_v += v_increments;

					}
				}
				else if (shorter == v_axis)
				{
					//average along u
					float v_increments = ((v2 + v4) / 2 - (v1 + v3) / 2) / num_samples;
					float h_increments = ((u2 + u4) / 2 - (u1 + u3) / 2) / num_samples;
					double r = 0, g = 0, b = 0, start_h = (u1 + u3) / 2 + 0.5 * h_increments, start_v = (v1 + v3) / 2 + 0.5 * h_increments;

					for (float s = 0; s < num_samples; s++)
					{
						//bilinearly interpolate in the same level
						BI(start_h, start_v, d);
						r += BI_res[0];
						g += BI_res[1];
						b += BI_res[2];

						start_h += h_increments;
						start_v += v_increments;


					}
					result[i] = r / num_samples;
					result[i + 1] = g / num_samples;
					result[i + 2] = b / num_samples;

				}
				else
				{
					//bilinearly interpolate on level 0
					BI(u, v, 0);
					result[i] = BI_res[0];
					result[i + 1] = BI_res[1];
					result[i + 2] = BI_res[2];

				}




			}
		}

	}
	//save result
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("anistropic_filter_BI.jpg", width, height, 3, result, 100);
}


// =============================================================================
// OPrnGL Display and Mouse Processing Functions.
//
// You can read up on OPrnGL and modify these functions, as well as the commands
// in main(), to Prrform more sophisticated display or GUI behavior. This code
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
int main(int argc, char* argv[])
{

	//initialize the global variables
	width = 256;
	height = 256;
	result = new unsigned char[256 * 256 * 3];
	BI_res = new unsigned char[3];

	int choice;
	string filterType;

	cout << "Choose a filter to use it's MipMaps:\n1.Box Filter with Square Footprint \n2. Box Filter with Square Footprint \n3.Tent Filter with Square Footprint \n4. Tent Filter with Circle Footprint \n5. Dilation Filter \n";
	cin >> choice;

	switch (choice)
	{
	case 1:
		filterType = "BoxSquare";
		break;

	case 2:
		filterType = "BoxCircle";
		break;
	case 3:
		filterType = "TentSquare";
		break;
	case 4:
		filterType = "TentCircle";
		break;
	case 5:
		filterType = "Dilation";
		break;
	default:
		cout << "Invalid Choice. Exiting now.";
		exit(0);
	}

	setPixels(filterType);

	// OPrnGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefiniteL_y to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(200, 200); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Homework Zero");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0; //This line never gets reached. We use it because "main" is T_yPr int.
}

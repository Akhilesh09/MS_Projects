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
#include <stb_include.h>

#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char* result;

int width = 200, height = 200, channels1, channels2, channels3;

int ind = 0;

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

	friend ostream& operator<<(ostream& os, Vector& v)
	{
		os << "(" << v.x << "," << v.y << "," << v.z << ")";
		return os;
	}


};

float r = 50;


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


float crop(float min, float max, float x) {
	x = (x - min) / (max - min);
	if (x > 1)
		x = 1;
	if (x < 0)
		x = 0;
	return -2 * pow(x, 3) + 3 * pow(x, 2);
}

//Parent class for different objects
class Object
{
public:
	Vector Pc;
	float r, ior;
	Vector P0, P1, P2;
	Vector A;
	Vector color;
	char type;
	bool reflective, refractive;
	Vector n;
	virtual void normal(Vector& P)
	{
	}
	virtual float eq(Vector& P)
	{
		return -1;
	}
	virtual void print()
	{

	}
	virtual bool intersect(Vector& P, Vector& dir, float& t_hit)
	{
		return false;
	}
};

//Child class for Sphere
class Sphere : public Object
{
public:
	//Constructor to intialize required members
	Sphere(float x, float y, float z, float radius, Vector& c, bool refl, bool refr, char t, float ir)
	{
		Pc.x = x;
		Pc.y = y;
		Pc.z = z;
		r = radius;
		color.x = c.x;
		color.y = c.y;
		color.z = c.z;
		reflective = refl;
		refractive = refr;
		type = t;
		ior = ir;

	}

	//compute normal at hitpoint
	void normal(Vector& P)
	{
		n = (P - Pc) * (1 / r);

	}

	//sphere equation
	float eq(Vector& P)
	{
		return (P - Pc) * (P - Pc) - pow(r, 2);
	}

	//print
	void print()
	{
		cout << "Center:" << Pc << endl;
	}

	//check ray-sphere intersection
	bool intersect(Vector& P, Vector& dir, float& t_hit)
	{
		float b, c, disc;
		b = dir * (P - Pc);
		c = eq(P);
		disc = sqrt(pow(b, 2) - c);
		if ((b > 0 || isnan(disc)))
		{

		}
		else
		{

			t_hit = -b - disc;
			return true;
		}
		return false;
	}



};

//Child class for Plane
class Plane : public Object
{
public:
	//Constructor to intialize required members
	Plane(Vector& P, Vector& normal, Vector& c, bool refl, bool refr, char t, float ir)
	{
		Pc.x = P.x;
		Pc.y = P.y;
		Pc.z = P.z;
		n = normal;
		color.x = c.x;
		color.y = c.y;
		color.z = c.z;
		reflective = refl;
		refractive = refr;
		type = t;
		ior = ir;

	}

	//dummy normal function
	void normal(Vector& P)
	{
		return;

	}

	//plane equation
	float eq(Vector& P)
	{


		return n * (P - Pc);
	}

	//print
	void print()
	{
		cout << "Point:" << Pc << endl;
	}

	//check ray-plane intersection
	bool intersect(Vector& P, Vector& dir, float& t_hit)
	{
		float num = (n * (Pc - P));
		float den = (n * dir);
		if (num < 0 && den < 0)
		{
			t_hit = num / den;

			Vector P_hit = P + (dir * t_hit);

			return true;
		}
		return false;
	}


};

//Child class for Triangle
class Triangle : public Object
{
public:
	//Constructor to intialize required members
	Triangle(Vector& S1, Vector& S2, Vector& S3, Vector& c, bool refl, bool refr, char t, float ir)
	{
		P0 = S1;
		P1 = S2;
		P2 = S3;
		type = t;
		color = c;
		reflective = refl;
		refractive = refr;
		ior = ir;
	}
	//compute face normal
	void normal(Vector& P)
	{

		Vector T_V0, T_V1, T_V2;

		T_V0 = P0 - P2;
		T_V1 = P1 - P0;
		T_V2 = P2 - P1;


		A = cross_product(T_V0, T_V1);
		A = A * 0.5;
		n = A * (1 / magnitude(A.x, A.y, A.z));

	}

	//print
	void print()
	{
		cout << "Face:" << P0 << "," << P1 << "," << P2 << endl;
	}

	//check ray-triangle intersection
	bool intersect(Vector& P, Vector& dir, float& t_hit)
	{

		float num = (n * (P1 - P));
		float den = (n * dir);
		float t;
		if (num < 0 && den < 0)
		{
			t_hit = num / den;

			Vector P_hit = P + (dir * t_hit);

			Vector T_A0, T_A1, T_A2;

			T_A0 = cross_product(P_hit - P2, P1 - P_hit);
			T_A0 = T_A0 * 0.5;
			T_A1 = cross_product(P_hit - P0, P2 - P_hit);
			T_A1 = T_A1 * 0.5;
			T_A2 = cross_product(P_hit - P1, P0 - P_hit);
			T_A2 = T_A2 * 0.5;


			float s, t, u;
			s = (n * T_A1) / magnitude(A.x, A.y, A.z);
			t = (n * T_A2) / magnitude(A.x, A.y, A.z);
			u = (n * T_A0) / magnitude(A.x, A.y, A.z);


			if (s > 0 && s < 1 && t>0 && t < 1 && u>0 && u < 1)
			{
				return true;
			}
		}
		return false;
	}
};



Object* objects[3];

//check if intersection with any object
int trace(Vector& P, Vector& dir, int& index, float& t_hit)
{
	float t_min = 999;
	float ind_min = 999;
	float t;

	for (int k = 0; k < ind; k++)
	{

		if (objects[k]->intersect(P, dir, t) && t < t_min) {
			ind_min = k;
			t_min = t;

		}

	}


	t_hit = t_min;
	index = ind_min;

	return t_hit != 999 && index != 999;
}

//maximum of two floats
float max(float a, float b)
{
	if (a > b)
		return a;
	return b;
}

//minimum of two floats
float min(float a, float b)
{
	if (a > b)
		return b;
	return a;
}


Vector castRay(Vector& Pe, Vector& npe, Vector& default_col, const int& depth = 0)
{

	Vector L;
	L.x = (60);
	L.y = (-100);
	L.z = (50);

	L = L * (1 / magnitude(L.x, L.y, L.z));
	Vector dark, n;
	dark.x = 0;
	dark.y = 0;
	dark.z = 0;

	Vector hit_col = dark;

	int index;
	float t_hit;
	Vector refl_col = dark;
	Vector refr_col = dark;

	if (depth > 4)
		return default_col;

	//if there is an intersection
	if (trace(Pe, npe, index, t_hit))
	{
		//hitpoint
		Vector P_hit = Pe + (npe * t_hit);

		objects[index]->normal(P_hit);
		Vector n = objects[index]->n;
		double T = 0.5 * (L * n) + 0.5;

		hit_col = objects[index]->color; //no intersection

	}
	else
		hit_col = default_col;

	return hit_col;

}



//setup
void setPixels()
{
	//base colors
	Vector tri_c1, tri_c2, tri_c3;
	tri_c1.x = 255;
	tri_c1.y = 0;
	tri_c1.z = 255;

	tri_c2.x = 0;
	tri_c2.y = 255;
	tri_c2.z = 255;

	tri_c3.x = 255;
	tri_c3.y = 255;
	tri_c3.z = 0;


	//default color for no intersection
	Vector default_col;
	default_col.x = 0;
	default_col.y = 0;
	default_col.z = 0;


	//camera setup
	//camera up vector
	Vector Vup;
	Vup.x = 1;
	Vup.y = 1;
	Vup.z = 1;

	//camera view direction
	Vector V_view;
	V_view.x = 0;
	V_view.y = -1;
	V_view.z = 0.5;

	//camera local normals
	Vector V0 = cross_product(V_view, Vup);
	Vector n0;
	n0 = V0 * (1 / magnitude(V0.x, V0.y, V0.z));
	Vector n2;
	n2 = V_view * (1 / magnitude(V_view.x, V_view.y, V_view.z));
	Vector n1 = cross_product(n0, n2);

	//eyepoint
	Vector Pe;
	Pe.x = 8;
	Pe.y = 15;
	Pe.z = -15;

	//camera dimensions and distance from eyepoint
	float d = 100, sx = 200;
	float sy = sx * height / width;

	//center of camera
	Vector P_Cam;
	P_Cam = Pe + (n2 * d);

	//bottom-left corner of camera
	Vector P00;
	P00 = P_Cam - (n0 * (sx / 2)) - (n1 * (sy / 2));

	//adding triangle objects
	Vector T_P0, T_P1, T_P2;
	//triangle vertices

	T_P0.x = 5;
	T_P0.y = -5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = -5;

	T_P2.x = 5;
	T_P2.y = -5;
	T_P2.z = 5;


	Triangle* t = new Triangle(T_P0, T_P1, T_P2, tri_c1, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;



	T_P0.x = 5;
	T_P0.y = -5;
	T_P0.z = 5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = -5;

	T_P2.x = -5;
	T_P2.y = -5;
	T_P2.z = 5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c2, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;


	T_P0.x = 5;
	T_P0.y = -5;
	T_P0.z = 5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = 5;

	T_P2.x = 5;
	T_P2.y = 5;
	T_P2.z = 5;


	t = new Triangle(T_P0, T_P1, T_P2, tri_c3, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;

	T_P0.x = 5;
	T_P0.y = 5;
	T_P0.z = 5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = 5;

	T_P2.x = -5;
	T_P2.y = 5;
	T_P2.z = 5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c3, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;

	//********************


	T_P0.x = 5;
	T_P0.y = 5;
	T_P0.z = -5;

	T_P1.x = 5;
	T_P1.y = 5;
	T_P1.z = 5;

	T_P2.x = 5;
	T_P2.y = -5;
	T_P2.z = 5;


	t = new Triangle(T_P0, T_P1, T_P2, tri_c1, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;



	T_P0.x = 5;
	T_P0.y = 5;
	T_P0.z = -5;

	T_P1.x = 5;
	T_P1.y = -5;
	T_P1.z = 5;

	T_P2.x = 5;
	T_P2.y = -5;
	T_P2.z = -5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c2, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;


	T_P0.x = -5;
	T_P0.y = -5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = 5;

	T_P2.x = -5;
	T_P2.y = 5;
	T_P2.z = 5;


	t = new Triangle(T_P0, T_P1, T_P2, tri_c3, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;

	T_P0.x = -5;
	T_P0.y = -5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = 5;
	T_P1.z = 5;

	T_P2.x = -5;
	T_P2.y = 5;
	T_P2.z = -5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c3, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;


	//*******************



	T_P0.x = 5;
	T_P0.y = -5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = -5;

	T_P2.x = 5;
	T_P2.y = 5;
	T_P2.z = -5;


	t = new Triangle(T_P0, T_P1, T_P2, tri_c1, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;



	T_P0.x = 5;
	T_P0.y = 5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = -5;
	T_P1.z = -5;

	T_P2.x = -5;
	T_P2.y = 5;
	T_P2.z = -5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c2, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;


	T_P0.x = 5;
	T_P0.y = 5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = 5;
	T_P1.z = -5;

	T_P2.x = -5;
	T_P2.y = 5;
	T_P2.z = 5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c3, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;

	T_P0.x = 5;
	T_P0.y = 5;
	T_P0.z = -5;

	T_P1.x = -5;
	T_P1.y = 5;
	T_P1.z = 5;

	T_P2.x = 5;
	T_P2.y = 5;
	T_P2.z = 5;

	t = new Triangle(T_P0, T_P1, T_P2, tri_c3, true, false, 't', 1.5);
	objects[ind] = t;
	objects[ind]->normal(Pe);
	ind++;


	//main raycasting loop
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int i = (y * width + x) * 3;
			Vector Pp, npe, P_hit, P_h;

			//point on camera plane
			Pp = P00 + (n0 * (sx * x / width)) + (n1 * (sy * y / height));

			//primary ray direction
			npe = Pp - Pe;
			npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

			Vector final_col;
			int count;
			final_col = castRay(Pe, npe, default_col);

			//output array
			result[i] = final_col.x;
			result[i + 1] = final_col.y;
			result[i + 2] = final_col.z;


		}
	}

	//write result to jpg
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("cube.jpg", 200, 200, 3, result, 100);
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
	width = 200;
	height = 200;
	result = new unsigned char[200 * 200 * 3];

	setPixels();

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

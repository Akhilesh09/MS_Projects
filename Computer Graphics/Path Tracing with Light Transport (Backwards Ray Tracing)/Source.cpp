#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_include.h>

#include<vector>
#include<iostream>
#include <cstdlib>

#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char* result;

int width = 512, height = 512, channels1, channels2, channels3;

int ind = 0;

default_random_engine generator,generator2;
std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
uniform_real_distribution<double> distribution(0.0, 1.0);
uniform_real_distribution<double> distribution2(-1.0, 1.0);

//illumination maps for each object
float illumination_map[5][256][256] = { 0 };




class Vector
{
public:
	float x, y, z;
	Vector()
	{
	}
	Vector(float x1, float y1, float z1)
	{
		x = x1, y = y1, z = z1;
	}
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
Vector light_energy_out(255, 255, 255);

Vector n_v2(25,-60,50);



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

float drand48()
{
	return distribution(generator);
	//return (float)rand() / (float)RAND_MAX;
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
		return 0;
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



Object* objects[6];

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

Vector reflect(const Vector& I, const Vector& N)
{
	return I - N * (2 * (I * N));
}




//dimensions for repeating illumination map on plane
//couldn't find how to repeat it just once
float p_sx = 30, p_sy = p_sx * height / width;

void bilinearPartition(Vector P_hit, int index, float u,float v)
{
	int u_p = u * (255);
	int v_p = v * (255);

	int Au = 0, Av = 0, Bu = 0, Bv = 0, Cu = 0, Cv = 0, Du = 0, Dv = 0;

	//find x of nearest texels
	for (float m = 0; m < 256; m++)
	{
		if (m > u_p)
		{
			Au = m - 1;
			Cu = Au;
			Bu = m;
			Du = Bu;
			break;
		}
	}

	//find y of nearest texels
	for (float n = 0; n < 256; n++)
	{
		if (n > v_p)
		{
			Av = n - 1;
			Bv = Av;
			Cv = n;
			Dv = Cv;
			break;
		}
	}

	//bilinear partition between 4 surrounding texels
	float denom = (Cv - v_p) * (Bu - u_p)+ (u_p - Au)+ (v_p - Av) + (Bu - u_p)+ (u_p - Au);
	illumination_map[index][Au][Av] += illumination_map[index][u_p][v_p] *( (Cv - v_p) * (Bu - u_p))/denom;
	illumination_map[index][Bu][Bv] += illumination_map[index][u_p][v_p] *( (u_p - Au))/denom;
	illumination_map[index][Cu][Cv] += illumination_map[index][u_p][v_p] *( (v_p - Av) + (Bu - u_p))/denom;
	illumination_map[index][Cu][Cv] += illumination_map[index][u_p][v_p] *( (u_p - Au))/denom;
	illumination_map[index][u_p][v_p] = 0;



}

float computeArea(float u,float v)
{
	//uv of surrounding texels
	float Au = 0, Av = 0, Bu = 0, Bv = 0, Cu = 0, Cv = 0, Du = 0, Dv = 0;

	//find u of nearest texels
	for (float m = 0; m <1; m+=0.004)
	{

		if (m > u)
		{
			Au = m - 0.004;
			Cu = Au;
			Bu = m;
			Du = Bu;
			break;
		}
	}

	//find v of nearest texels
	for (float n = 0; n < 1; n+=0.004)
	{
		if (n > v)
		{
			Av = n - 0.004;
			Bv = Av;
			Cv = n;
			Dv = Cv;
			break;
		}
	}

	Au *= 255;
	Av *= 255;
	Bu *= 255;
	Bv *= 255;
	Cu *= 255;
	Cv *= 255;
	Du *= 255;
	Dv *= 255;

	//area of polygon from 4 corners
	float area = abs(((Au * Bv - Bu * Av) + (Bu * Dv - Du * Bv) + (Du * Cv - Cu * Dv) + (Cu * Av - Au * Cv)) / 2);

	return area;
}

//Light position
//Vector Pl(600, 0, 0
Vector Pl;
//Light direction
Vector L;

Vector castRay(Vector& Pe, Vector& npe, Vector& default_col, float alpha, const int& depth = 0)
{

	Vector r(0, 0.2, 0);
	r = r * (1 / magnitude(r.x, r.y, r.z));

	Vector dark(0, 0, 0), n;

	Vector hit_col = dark;

	int index;
	float t_hit;
	Vector refl_col = dark;
	Vector refr_col = dark;



	float importance_drop = alpha * (1 - alpha);

	if (depth > 1)
	{

		//N=5
		//If importance < 0.2, play Russian Roulette
		if (alpha < 0.2)
		{

			double number = distribution(generator);

			// if value > 0.2, kill path (return black color)
			if (number > 0.2)
			{

				return dark;
			}
			else
				importance_drop *= 5;
		}
	}

	//if there is an intersection
	if (trace(Pe, npe, index, t_hit))
	{

		//hitpoint
		Vector P_hit = Pe + (npe * t_hit);

		objects[index]->normal(P_hit);
		Vector n = objects[index]->n;

		//N.L
		bool outside = npe * n < 0;
		Vector bias = n * 0.001;

		Vector dir = P_hit - Pl;
		dir = dir * (1 / magnitude(dir.x, dir.y, dir.z));

		double T = 0.5 * (dir * n) + 0.5;

		int ind;
		float t_h;
		float area;

		float u, v;

		if (objects[index]->reflective == false)
		{

			if (objects[index]->type == 'p')
			{

				n_v2 = n_v2 * (1 / magnitude(n_v2.x, n_v2.y, n_v2.z));

				Vector n_p2 = cross_product(n, n_v2);

				Vector n_p3 = cross_product(n, n_p2);

				u = n_p2 * (P_hit - objects[index]->Pc) * (1 / p_sx);
				v = n_p3 * (P_hit - objects[index]->Pc) * (1 / p_sy);

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
			}
			else if (objects[index]->type == 's' && objects[index]->reflective == false)
			{
				float x_hit = (P_hit.x - objects[index]->Pc.x) * (1 / objects[index]->r);
				float y_hit = (P_hit.y - objects[index]->Pc.y) * (1 / objects[index]->r);
				float z_hit = (P_hit.z - objects[index]->Pc.z) * (1 / objects[index]->r);

				float phi = acos(z_hit);
				v = phi / 3.14;

				float theta = acos(y_hit / sin(phi));

				if (x_hit < 0)
					theta = 2 * 3.14 - theta;

				u = theta / (2 * 3.14);
			}


			area = computeArea(u, v);
		}

		if (objects[index]->reflective) //if object is reflective
		{
			//reflection
			Vector n_refl = reflect(npe, n);

			//sampling random point on unit sphere

			double theta = distribution(generator) * 2 * 3.14;
			double z = distribution2(generator2) ;

			Vector rand(sqrt(1 - pow(z, 2)) * cos(theta), sqrt(1 - pow(z, 2)) * sin(theta),z);

			n_refl = n_refl + rand*0.1;
			n_refl = n_refl * (1 / magnitude(n_refl.x, n_refl.y, n_refl.z));
			P_hit = outside ? P_hit + bias : P_hit - bias;
			hit_col = hit_col + castRay(P_hit, n_refl, default_col, importance_drop, depth + 1);

		}

		else
		{
			//////shadow ray
			
			dir = dir * -1;
			if (trace(P_hit, dir, ind, t_h) && ind != index)
			{
				T = 1;

			}
			else if (L * dir - cos(60 * 3.14 / 180) < 0) // if outside spot light cone
			{
				T = 1;
			}
			hit_col = hit_col + (objects[index]->color * (1 - T));
			
			
			if (isnan(u))
				u = 0;
			if (isnan(v))
				v = 0;

			// value from illumination map times Intensity / area
			Vector temp = (light_energy_out * (illumination_map[index][(int)(u * 255)][(int)(v * 255)]/ (area)));

			if (temp.x >= 0 && temp.y >= 0 && temp.z >= 0)
			{
				//multiply by diffuse
				hit_col = hit_col * temp.x*700000000;
			}

		}
		

	}
	else
		hit_col = default_col;

	return hit_col;

}





void castLight(Vector& Pe, Vector& npe, Vector& default_col, float energy_out, const int& depth = 0)
{

	Vector n;
	int index;
	float t_hit;

	n_v2 = n_v2 * (1 / magnitude(n_v2.x, n_v2.y, n_v2.z));


	if (depth > 5)
		return;

	//if there is an intersection
	if (trace(Pe, npe, index, t_hit))
	{
		//hitpoint
		Vector P_hit = Pe + (npe * t_hit);

		objects[index]->normal(P_hit);
		Vector n = objects[index]->n;

		Vector n_p2 = cross_product(n, n_v2);

		Vector n_p3 = cross_product(n, n_p2);
		

		float u,v;

		//illumination phase only for dis=ffuse objects
		if (objects[index]->type == 's' && objects[index]->reflective == false)
		{
			float x_hit = (P_hit.x - objects[index]->Pc.x) * (1 / objects[index]->r);
			float y_hit = (P_hit.y - objects[index]->Pc.y) * (1 / objects[index]->r);
			float z_hit = (P_hit.z - objects[index]->Pc.z) * (1 / objects[index]->r);

			float phi = acos(z_hit);
			v = phi / 3.14;

			float theta = acos(y_hit / sin(phi));

			if (x_hit < 0)
				theta = 2 * 3.14 - theta;

			u = theta / (2 * 3.14);

			if (isnan(u))
				u = 0;
			if (isnan(v))
				v = 0;

			//assign energy to hitpoint UV in illumination map; storing N.L
			illumination_map[index][(int)(u * 255)][(int)(v * 255)] += 0.2 * (n * npe);
			//bilinearly partiiton energy among 4 surrounding texels
			bilinearPartition(P_hit, index, u, v);
			//decrease energy of ray for next bounce
			energy_out *= 0.8;
		}
		else if (objects[index]->type == 'p')
		{
			u = n_p2 * (P_hit - objects[index]->Pc) * (1 / p_sx);
			v = n_p3 * (P_hit - objects[index]->Pc) * (1 / p_sy);

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

			

			illumination_map[index][(int)(u * 255)][(int)(v * 255)] += 0.2 * (n * npe);
			bilinearPartition(P_hit, index, u, v);
			energy_out *= 0.8;
		}

		
		bool outside = npe * n < 0;
		Vector bias = n * 0.001;

		double phi = drand48() * 2 * 3.14;
		//random angle between -1 and 1
		double costheta = 2 * drand48() - 1;

		double theta = std::acos(costheta);
		Vector rand(sin(theta) * cos(phi),sin(theta) * sin(phi),cos(theta));

		P_hit = outside ? P_hit + bias : P_hit - bias;
		castLight(P_hit, rand, default_col, energy_out, depth + 1);

	}

	return ;

}



//setup
void setPixels()
{

	ifstream inFile;

	//variables to store file data
	string type;
	string reflective;
	string refractive,ior;
	string color_r,color_g,color_b;
	string pos_x,pos_y,pos_z;
	string radius;
	string norm_x, norm_y, norm_z;

	//Vector sph1_c(255, 255, 0), sph2_c(255, 0, 255), sph3_c(255, 0, 0), p1_c(0, 0, 255), p2_c(0, 255, 255);


	////plane points and normals
	//Vector P_p1(0, 0, -150);

	//Vector P_p2(-50, 0, -150);

	//Vector P_p3(-50, 0, -150);


	//Vector pn2(0, 1, 1), pn1(1, 0, 1), pn3(1, 0, 0);
	//pn1 = pn1 * (1 / magnitude(pn1.x, pn1.y, pn1.z));

	//pn2 = pn2 * (1 / magnitude(pn2.x, pn2.y, pn2.z));

	////Adding sphere objects

	//Sphere* sp = new Sphere(0, 0, -40, 16.5, sph3_c, true, false, 's', 1.66);
	//objects[ind] = sp;
	//ind += 1;

	//sp = new Sphere(-50, 20, -50, 16.5, sph1_c, false, false, 's', 1);
	//objects[ind] = sp;
	//ind += 1;

	//sp = new Sphere(50, 0, -50, 16.5, sph2_c, false, false, 's', 1);
	//objects[ind] = sp;
	//ind += 1;


	//////adding plane objects
	//Plane* p = new Plane(P_p2, pn2, p2_c, false, false, 'p', 1);
	//objects[ind] = p;
	//ind += 1;

	//p = new Plane(P_p1, pn1, p1_c, false, false, 'p', 1);
	//objects[ind] = p;
	//ind += 1;


	inFile.open("scene_info.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	while (1) {
		getline(inFile, type, ',');
		if (type == "s")
		{
			getline(inFile, pos_x, ',');
			getline(inFile, pos_y, ',');
			getline(inFile, pos_z, ',');
			getline(inFile, radius, ',');
			getline(inFile, color_r, ',');
			getline(inFile, color_g, ',');
			getline(inFile, color_b, ',');
			getline(inFile, reflective, ',');
			getline(inFile, refractive, ',');
			getline(inFile, ior, '\n');
			Vector sph_col(stof(color_r), stof(color_g), stof(color_b));
			Sphere* sp = new Sphere(stof(pos_x), stof(pos_y), stof(pos_z), stof(radius), sph_col, reflective == "TRUE", refractive == "TRUE", 's', stof(ior));
			objects[ind] = sp;
			ind += 1;
		}
		else if (type == "p")
		{
			getline(inFile, pos_x, ',');
			getline(inFile, pos_y, ',');
			getline(inFile, pos_z, ',');
			getline(inFile, norm_x, ',');
			getline(inFile, norm_y, ',');
			getline(inFile, norm_z, ',');
			getline(inFile, color_r, ',');
			getline(inFile, color_g, ',');
			getline(inFile, color_b, '\n');
			Vector Plane_pt(stof(pos_x), stof(pos_y), stof(pos_z));
			Vector Plane_norm(stof(norm_x), stof(norm_y), stof(norm_z));
			Vector Plane_col(stof(color_r), stof(color_g), stof(color_b));
			Plane* p = new Plane(Plane_pt, Plane_norm, Plane_col, false, false, 'p', 1);
			objects[ind] = p;
			ind += 1;

		}
		else if (type == "l")
		{
			getline(inFile, pos_x, ',');
			getline(inFile, pos_y, ',');
			getline(inFile, pos_z, '\n');
			Pl = Vector(stof(pos_x), stof(pos_y), stof(pos_z));

		}
		
		/*getline(inFile, ior, '\n');
		try {
			mat_wavelengths[i] = stof(wavelength) * 1000;
			mat_iors[i] = stof(ior);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}*/

		if (inFile.eof())
			break;
	}
	inFile.close();


	//base colors
	

	//default color for no intersection
	Vector default_col(0, 0, 0);


	//camera setup
	//camera up vector
	Vector Vup(0, 1, 0);

	//camera view direction
	Vector V_view(0, 0, -1);

	//camera local normals
	Vector V0 = cross_product(V_view, Vup);
	Vector n0;
	n0 = V0 * (1 / magnitude(V0.x, V0.y, V0.z));
	Vector n2;
	n2 = V_view * (1 / magnitude(V_view.x, V_view.y, V_view.z));
	Vector n1 = cross_product(n0, n2);

	//eyepoint
	Vector Pe(0, 0, 0);

	//camera dimesnsions and distance from eyepoint
	float d = 100, sx = 500;
	float sy = sx * height / width;

	//center of camera
	Vector P_Cam;
	P_Cam = Pe + (n2 * d);

	//bottom-left corner of camera
	Vector P00;
	P00 = P_Cam - (n0 * (sx / 2)) - (n1 * (sy / 2));

	//N = 5, Alpha = 1/N
	float alpha = 0.2, num_samples = 20;


	int num_light_rays = 2*pow(10,9);

	//Illumination phase
	for (int n = 0; n < num_light_rays; n++)
	{
		// sample light ray directions from unit sphere

		double theta = ((n%num_light_rays)/num_light_rays) * 2 * 3.14;
		double z = ((n / num_light_rays) / num_light_rays)*2-1;

		/*double theta = distribution(generator) * 2 * 3.14;
		double z = distribution2(generator2);*/

		Vector rand(sqrt(1 - pow(z, 2)) * cos(theta), sqrt(1 - pow(z, 2)) * sin(theta), z);

		L = Pl * (1 / magnitude(Pl.x, Pl.y, Pl.z));
		L = L + rand;
		L = L * (1 / magnitude(L.x, L.y, L.z));

		castLight(Pl, L, default_col, (1.0/num_light_rays), 0);

	}

	//main raycasting loop
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int i = (y * width + x) * 3;
			Vector final_col(0, 0, 0);

			L = Pl * (1 / magnitude(Pl.x, Pl.y, Pl.z));

			for (float m = 0; m < num_samples; m++)
			{
				for (float n = 0; n < num_samples; n++)
				{

					Vector Pp, npe, P_hit, P_h;

					//point on camera plane
					Pp = P00 + (n0 * (sx * ((x + m / num_samples) / width))) + (n1 * (sy * ((y + n / num_samples) / height)));

					//primary ray direction
					npe = Pp - Pe;
					npe = npe * (1 / magnitude(npe.x, npe.y, npe.z));

					final_col = final_col + castRay(Pe, npe, default_col, alpha, 0);

				}

			}
			//cout << x << endl << y << endl;
			//output array
			result[i] = final_col.x / pow(num_samples, 2);
			result[i + 1] = final_col.y / pow(num_samples, 2);
			result[i + 2] = final_col.z / pow(num_samples, 2);
		}
	}

	//write result to jpg
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("path_tracer.jpg", 512, 512, 3, result, 100);

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
	width = 512;
	height = 512;
	result = new unsigned char[512 * 512 * 3];

	setPixels();

	// OPrnGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefiniteL_y to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(512, 512); // Where the window will display on-screen.
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

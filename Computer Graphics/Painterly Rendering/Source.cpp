#include <cstdlib>
#include <iostream>

#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_include.h"

#include<vector>
#include<iostream>
#include <cstdlib>

#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char* input_img;
unsigned char* ref_img;
unsigned char* diff_img;
unsigned char* canvas;

//spline points
vector<int> spline_x;
vector<int> spline_y;
vector<int> spline_index;

int width = 458, height = 288, channels;

int brushSizes[5];

//sobel filters
double sobel_y[3][3] = { {1,2,1 },{0,0,0}, { -1,-2,-1} };
double sobel_x[3][3] = { {1,0,-1},{2,0,-2},{1,0,-1} };

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

//vector magnitude
float norm(Vector a)
{

	return pow(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2), 0.5);
}

// =============================================================================
// setPixels()
//
// This function stores the RGB values of each pixel to "pixmap."
// Then, "glutDisplayFunc" below will use pixmap to display the pixel colors.
// =============================================================================



Vector cross_product(Vector a, Vector b)
{
	Vector res;
	res.x = a.y * b.z - b.y * a.z;
	res.y = a.z * b.x - b.z * a.x;
	res.z = a.x * b.y - b.x * a.y;
	return res;
}

//color difference
float difference(int r1, int g1, int b1, int r2, int g2, int b2) {
	return pow(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2), 0.5);
}

//makes strokes as circles centered at x1,y1 of radius=brushRadius
void makeStroke(int brushRadius,int x1,int y1)
{
	int col_ind = (y1*width + x1) * 3;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			
			if(pow(x-x1,2)+pow(y-y1,2)-pow(brushRadius,2)<=0)
			{
					int k = (y * width + x) * 3;


					canvas[k] = input_img[col_ind];
					canvas[k + 1] = input_img[col_ind + 1];
					canvas[k + 2] = input_img[col_ind + 2];

			}

		}
	}


}

//sobel filter x-direction
int gradient_x(int x, int y)
{
	int N = 3;

	double q, p, o;
	q = 0, p = 0, o = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {

			float u = x + i - 0.5 * (N - 1);
			float v = y + j - 0.5 * (N - 1);

			if (u < 0) {
				u = abs(u);
			}
			else if (u >= width) {
				u = width - (u - width);
			}
			if (v < 0) {
				v = abs(v);
			}
			else if (v >= height) {
				v = height - (v - height);
			}

			int a = (v * width + u) * 3;
			p += ref_img[a++] * sobel_x[i][j];
			q += ref_img[a++] * sobel_x[i][j];
			o += ref_img[a++] * sobel_x[i][j];
		}
	}
	Vector grad(q, p, o);
	return norm(grad);


}

//sobel filter y-direction
int gradient_y(int x, int y)
{
	int N = 3;

	double q, p, o;
	q = 0, p = 0, o = 0;
	for (int i = 0; i < N; i++) {
	for (int j = 0; j < N; j++) {

		float u = x + i - 0.5 * (N - 1);
		float v = y + j - 0.5 * (N - 1);

		if (u < 0) {
			u = abs(u);
		}
		else if (u >= width) {
			u = width - (u - width);
		}
		if (v < 0) {
			v = abs(v);
		}
		else if (v >= height) {
			v = height - (v - height);
		}

		int a = (v * width + u) * 3;
		p += ref_img[a++] * sobel_y[i][j];
		q += ref_img[a++] * sobel_y[i][j];
		o += ref_img[a++] * sobel_y[i][j];
	}
}
	Vector grad(q, p, o);
	return norm(grad);

}

//finds points on the spline to draw a stroke along
void makeSplineStroke(int brushRadius, int x1, int y1)
{


	spline_x.push_back(x1);
	spline_y.push_back(y1);

	int k = (y1 * width + x1) * 3;

	Vector strokeCol(ref_img[k], ref_img[k + 1], ref_img[k + 2]);

	int curr_x = x1, curr_y = y1;
	int max_stroke_length = 16, min_stroke_length = 4;
	int lastDx = 0, lastDy = 0;

	for (int s = 1; s <= max_stroke_length; s++) {

		int l = (curr_y * width + curr_x) * 3;

		Vector canvasCol(canvas[l], canvas[l + 1], canvas[l + 2]);
		Vector refCol(ref_img[l], ref_img[l + 1], ref_img[l + 2]);

		if (s > min_stroke_length && (norm(refCol - canvasCol)) < (norm(refCol - strokeCol)))
			return;

		int grad_x = gradient_x(curr_x, curr_y), grad_y = gradient_y(curr_x, curr_y);

		if (pow(grad_x, 2) + pow(grad_y, 2) == 0)
			return;

		int dx = -grad_y, dy = grad_x;

		if (lastDx * dx + lastDy * dy < 0)
		{
			dx *= -1;
			dy *= -1;
		}

		int mag = pow(pow(dx, 2) + pow(dy, 2), 0.5);

		int dx_norm = dx / mag, dy_norm = dy / mag;

		curr_x = curr_x + brushRadius * dx_norm;
		curr_y = curr_y + brushRadius * dy_norm;

		if (curr_x<0 || curr_x>width || curr_y<0 || curr_y>height)
			return;

		lastDx = dx_norm, lastDy = dy_norm;

		spline_x.push_back(curr_x);
		spline_y.push_back(curr_y);

	}


}


void paintLayer(int brushRadius)
{

	vector<vector<int>> spline_x_all;
	vector<vector<int>> spline_y_all;
	vector<int> spline_index;

	//Pointwise difference image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int k = (y * width + x) * 3;
			int l = (y * width + x);
			diff_img[l] = difference(canvas[k], canvas[k + 1], canvas[k + 2], ref_img[k], ref_img[k + 1], ref_img[k + 2]);
		}
	}

	int grid = brushRadius;

	//threshold
	int T = 10;
	int count = 0;

	for (int x = 0; x < width; x += grid)
	{
	for (int y = 0; y < height; y += grid)
	{
		
			int sum_error = 0;
			int max_error = -999, max_x, max_y;

			for (int i = 0; i < grid; i++)
			{
				for (int j = 0; j < grid; j++)
				{
					int c = y + i;
					int d = x + j;
					int a = (c * width + d);
					sum_error += diff_img[a];
					if (diff_img[a] > max_error)
					{
						max_error = diff_img[a];
						max_x = d;
						max_y = c;
					}
				}
			}

			float area_error = sum_error / pow(grid, 2);

			if (area_error > T)
			{
				makeSplineStroke(brushRadius, max_x, max_y);
			}

			// stroke indices to randomize
			spline_index.push_back(count++);

			//store strokes to apply randomly
			spline_x_all.push_back(spline_x);
			spline_y_all.push_back(spline_y);

			//clear for each stroke
			spline_x.clear();
			spline_y.clear();


	}
	}


	//shuffle strokes at random
	random_shuffle(spline_index.begin(), spline_index.end());

	for (int i = 0; i < spline_x_all.size(); i++)
	{
		vector<int> curr_x = spline_x_all.at(spline_index.at(i));
		vector<int> curr_y = spline_y_all.at(spline_index.at(i));

		for (int j = 0; j < curr_x.size(); j++)
		{
			makeStroke(brushRadius, curr_x.at(j), curr_y.at(j));

			//write canvas
			stbi_flip_vertically_on_write(true);
			stbi_write_jpg("painted.jpg", 458, 288, 3, canvas, 100);
		}
	}

	spline_x_all.clear();
	spline_y_all.clear();
	spline_index.clear();

}





void paint(int brushRadii[5])
{

	//for each brush size
	for (int i = 0; brushRadii[i] != 0; i++)
	{

		int radius = 0.5 * brushRadii[i];
		int N = radius * 2 + 1;

		//calculate Weights
		float weights[100][100];
		float total = 0;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				float u = i - 0.5 * (N - 1);
				float v = j - 0.5 * (N - 1);
				float weight = exp(-1 * ((u * u + v * v) / (radius * radius)));
				weights[i][j] = weight;
				total += weight;
			}
		}

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				weights[i][j] = weights[i][j] / total;
			}
		}

		//Gaussian blur input image

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				int k = (y * width + x) * 3;
				double r = 0, g = 0, b = 0;

				for (int i = 0; i < N; i++) {
					for (int j = 0; j < N; j++) {

						float u = x + i - 0.5 * (N - 1);
						float v = y + j - 0.5 * (N - 1);

						if (u < 0) {
							u = abs(u);
						}
						else if (u >= width) {
							u = width - (u - width);
						}
						if (v < 0) {
							v = abs(v);
						}
						else if (v >= height) {
							v = height - (v - height);
						}

						int a = (v * width + u) * 3;
						r += input_img[a++] * weights[i][j];
						g += input_img[a++] * weights[i][j];
						b += input_img[a] * weights[i][j];
					}
				}
				//reference image = gaussian blurred input image
				ref_img[k++] = r;
				ref_img[k++] = g;
				ref_img[k++] = b;
			}
		}
		 paintLayer(brushRadii[i]);

	}


}



//setup
void setPixels()
{

	//read input image
	stbi_set_flip_vertically_on_load(true);
	input_img = stbi_load("input.png", &width, &height, &channels, STBI_rgb);

	//blank black canvas
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int k = (y * width + x) * 3;
			canvas[k] = 0;
			canvas[k + 1] = 0;
			canvas[k + 2] = 0;
		}
	}
	
	//brush sizes
	//brushSizes[0] = 64;
	brushSizes[0] = 8;
	brushSizes[1] = 4;
	brushSizes[2] = 2;
	paint(brushSizes);

}

// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char* argv[])
{

	//initialize the global variables
	width = 458;
	height = 288;
	input_img = new unsigned char[width * height * 3];
	ref_img = new unsigned char[width * height * 3];
	diff_img = new unsigned char[width * height];
	canvas = new unsigned char[width * height * 3];

	setPixels();

	return 0; 
}

// Skeleton code: CSCE 641/VIZA 672 Assignment 2
// Basic ray casting (no secondary rays), with function to be filled in to do lighting calculation
// Renders 6 spheres in 2 rows of 3, with a consistent light direction on each.
// Outputs a .bmp file, with name "Image.bmp" by default; use first command-line parameter to set a different name

#include<fstream>
#include<vector>
#include<cmath>
#include<iostream>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

// Image Width and Height
int ImageW;
int ImageH;

ifstream inFile;

string wavelength;
string intensity;
double l_wavelengths[1500], intensities[1500];



string xbar;
string ybar;
string zbar;
double wavelengths[100], xbars[100], ybars[100], zbars[100];

string ior;
double mat_wavelengths[100], mat_iors[100], mat_ecoeffs[100];

string ecoeff;
double sample_intensities[13] = { 750,994.0,1317,1419.0,1425,1394.0,1332,1347.0,1325,1304.0,1289,1257.0,1220 }, sample_wavelengths[50];
//double sample_intensities[50] , sample_wavelengths[50];
double sample_ior[50], sample_ecoeff[50]; // gold
double sample_xbars[50], sample_ybars[50], sample_zbars[50];
double silver_sample_ior[50], silver_sample_ecoeff[50];
double copper_sample_ior[50], copper_sample_ecoeff[50];
double chromium_sample_ior[50], chromium_sample_ecoeff[50];

int s_max = 13;

//framebuffer[i][j] is the i-th row, counting from top, j-th column counting from left
vector<vector<vector<double>>> framebuffer;

struct Coord3D { double x, y, z, m; };

Coord3D Light;
Coord3D SphCent[6];
double SphRad[6];

// Normalizes the vector passed in
void normalize(double& x, double& y, double& z) {
	double temp = sqrt(x * x + y * y + z * z);
	if (temp > 0.0) {
		x /= temp;
		y /= temp;
		z /= temp;
	}
	else {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
}

// Returns dot product of two vectors
double dot(double x1, double y1, double z1, double x2, double y2, double z2) {
	return (x1 * x2 + y1 * y2 + z1 * z2);
}
double dot(Coord3D a, Coord3D b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Returns angle between two vectors
double angle(double x1, double y1, double z1, double x2, double y2, double z2) {
	normalize(x1, y1, z1);
	normalize(x2, y2, z2);
	return  acos(dot(x1, y1, z1, x2, y2, z2));
}

Coord3D operator+(Coord3D a, Coord3D b)
{
	Coord3D res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

Coord3D operator*(Coord3D a, double b)
{
	Coord3D res;
	res.x = a.x * b;
	res.y = a.y * b;
	res.z = a.z * b;
	return res;
}


double max(double a, double b)
{
	if (a > b)
		return a;
	return b;
}

//minimum of two doubles
double min(double a, double b)
{
	if (a > b)
		return b;
	return a;
}

//minimum of three doubles
double min(double a, double b, double c)
{
	if (a < b && a < c)
		return a;
	else if (b < a && b < c)
		return b;
	return c;
}

void clamp(double& val, double min, double max)
{
	if (val < min)
		val = min;
	else if (val > max)
		val = max;
}

// Outputs a .bmp file
// fname is file name
// a[i][j][k] : i is the row, starting from bottom; j is column, starting from left; k is R, G, B
void outputBMP(const char* fname, vector<vector<vector<double>>>& a) {
	fstream outfi(fname, fstream::out | fstream::binary);
	const int headersize = 14;
	const int infosize = 40;
	unsigned char bmpheader[headersize];
	unsigned char bmpinfo[infosize];
	int i, j;
	int h = a.size();
	int w = a[0].size();
	for (i = 0; i < headersize; i++) bmpheader[i] = 0;
	for (i = 0; i < infosize; i++) bmpinfo[i] = 0;

	//Padding: a "row" of pixels must have 4n bytes
	int padbytes = (4 - ((w * 3) % 4)) % 4;

	// SET UP HEADER
	// Starts with "BM"
	bmpheader[0] = 'B';	bmpheader[1] = 'M';
	int totalsize = h * (w * 3 + padbytes); // Size of array of pixels
	totalsize += headersize + infosize; // Now is real total
	// Put total file size in header; is little-endian over 4 bytes
	bmpheader[2] = (unsigned char)((totalsize >> 0) & 255);
	bmpheader[3] = (unsigned char)((totalsize >> 8) & 255);
	bmpheader[4] = (unsigned char)((totalsize >> 16) & 255);
	bmpheader[5] = (unsigned char)((totalsize >> 24) & 255);
	// Put offset to pixels into header; is little-endian over 4 bytes
	totalsize = headersize + infosize;
	bmpheader[10] = (unsigned char)((totalsize >> 0) & 255);
	bmpheader[11] = (unsigned char)((totalsize >> 8) & 255);
	bmpheader[12] = (unsigned char)((totalsize >> 16) & 255);
	bmpheader[13] = (unsigned char)((totalsize >> 24) & 255);

	// SET UP INFO BLOCK - using 40 bit BITMAPINFOHEADER format here
	// First four bytes are size of info block, little-endian
	bmpinfo[0] = (unsigned char)((infosize >> 0) & 255);
	bmpinfo[1] = (unsigned char)((infosize >> 8) & 255);
	bmpinfo[2] = (unsigned char)((infosize >> 16) & 255);
	bmpinfo[3] = (unsigned char)((infosize >> 24) & 255);
	// Next four are width
	bmpinfo[4] = (unsigned char)((a[0].size() >> 0) & 255);
	bmpinfo[5] = (unsigned char)((a[0].size() >> 8) & 255);
	bmpinfo[6] = (unsigned char)((a[0].size() >> 16) & 255);
	bmpinfo[7] = (unsigned char)((a[0].size() >> 24) & 255);
	// Next four are height
	bmpinfo[8] = (unsigned char)((a.size() >> 0) & 255);
	bmpinfo[9] = (unsigned char)((a.size() >> 8) & 255);
	bmpinfo[10] = (unsigned char)((a.size() >> 16) & 255);
	bmpinfo[11] = (unsigned char)((a.size() >> 24) & 255);
	// Number of color planes (must be 1)
	bmpinfo[12] = (unsigned char)1; bmpinfo[13] = 0;
	// 24 bits per pixel
	bmpinfo[14] = (unsigned char)24; bmpinfo[15] = 0;
	// No compression, so leave bits 16-19 == 0
	// Remaining bits can all be 0, since no compression/scale/important colors

	outfi.write((char*)bmpheader, headersize);
	outfi.write((char*)bmpinfo, infosize);
	unsigned char rgb[3];
	unsigned char padding[3] = { 0,0,0 };
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// Order is b, g, r in BMP, so reverse order here
			rgb[2] = (unsigned char)(a[i][j][0] * 255);
			rgb[1] = (unsigned char)(a[i][j][1] * 255);
			rgb[0] = (unsigned char)(a[i][j][2] * 255);
			outfi.write((char*)rgb, 3);
		}
		outfi.write((char*)padding, padbytes);
	}
	outfi.close();
}


float chiGGX(float v)
{
	return v > 0 ? 1 : 0;
}

//************* THIS IS THE ROUTINE YOU WOULD NEED TO WRITE *****************
// Get Color for a point on the surface
void GetColor(
	Coord3D view,   // Normalized Vector pointing FROM eye TO surface
	Coord3D normal, // Normalized Vector giving surface normal
	Coord3D light,  // Normalized Vector pointing FROM surface TO light
	int SphNum,     // Sphere Number (0-5)
	double& R,       // Return these values for surface color.
	double& G,
	double& B) {

	view = view * -1;

	Coord3D halfv;
	halfv = (view + light);
	normalize(halfv.x, halfv.y, halfv.z);
	double F, F0, D, Geo, specR;
	double Rs0, Rp0, Rs, Rp;

	double cosi = dot(normal, light);
	double X = 0, Y = 0, Z = 0, count = 0;

	double alpha = acos(dot(normal, halfv));

	//GGX Distribution
	double den = pow(dot(normal, halfv),2) * pow(SphCent[SphNum].m,2) + (1 - pow(dot(normal, halfv), 2));
	D = (chiGGX(dot(normal, halfv)) * pow(SphCent[SphNum].m, 2)) / (3.14*pow(den,2));


	Geo = min(1, (2 * dot(normal, halfv) * dot(normal, view)) / dot(view, halfv), (2 * dot(normal, halfv) * dot(normal, light)) / dot(view, halfv));

	for (int s = 0; s < s_max; s++)
	{
		if (SphNum == 0 || SphNum == 1 || SphNum == 2)
		{

			F = (pow(sample_ior[s] - 1, 2) + 4 * sample_ior[s] * pow(1 - cosi, 5) + pow(sample_ecoeff[s], 2)) / (pow(sample_ior[s] + 1, 2) + pow(sample_ecoeff[s], 2));
			F0 = (pow(sample_ior[s] - 1, 2) + 4 * sample_ior[s] * pow(1 - cos(0), 5) + pow(sample_ecoeff[s], 2)) / (pow(sample_ior[s] + 1, 2) + pow(sample_ecoeff[s], 2));


		}
		else if (SphNum == 3)
		{

			F = (pow(silver_sample_ior[s] - 1, 2) + 4 * silver_sample_ior[s] * pow(1 - cosi, 5) + pow(silver_sample_ecoeff[s], 2)) / (pow(silver_sample_ior[s] + 1, 2) + pow(silver_sample_ecoeff[s], 2));
			F0 = (pow(silver_sample_ior[s] - 1, 2) + 4 * silver_sample_ior[s] * pow(1 - cos(0), 5) + pow(silver_sample_ecoeff[s], 2)) / (pow(silver_sample_ior[s] + 1, 2) + pow(silver_sample_ecoeff[s], 2));

		}

		else if (SphNum == 4)
		{

			F = (pow(copper_sample_ior[s] - 1, 2) + 4 * copper_sample_ior[s] * pow(1 - cosi, 5) + pow(copper_sample_ecoeff[s], 2)) / (pow(copper_sample_ior[s] + 1, 2) + pow(copper_sample_ecoeff[s], 2));
			F0 = (pow(copper_sample_ior[s] - 1, 2) + 4 * copper_sample_ior[s] * pow(1 - cos(0), 5) + pow(copper_sample_ecoeff[s], 2)) / (pow(copper_sample_ior[s] + 1, 2) + pow(copper_sample_ecoeff[s], 2));


		}

		else
		{

			F = (pow(chromium_sample_ior[s] - 1, 2) + 4 * chromium_sample_ior[s] * pow(1 - cosi, 5) + pow(chromium_sample_ecoeff[s], 2)) / (pow(chromium_sample_ior[s] + 1, 2) + pow(chromium_sample_ecoeff[s], 2));
			F0 = (pow(chromium_sample_ior[s] - 1, 2) + 4 * chromium_sample_ior[s] * pow(1 - cos(0), 5) + pow(chromium_sample_ecoeff[s], 2)) / (pow(chromium_sample_ior[s] + 1, 2) + pow(chromium_sample_ecoeff[s], 2));


		}



		specR = ((F * D * Geo) / (3.14 * dot(normal, light) * dot(normal, view)));

		X += (sample_intensities[s] * (0.3 * specR + 0.7 * F0) * dot(normal, light)) * sample_xbars[s];
		Y += (sample_intensities[s] * (0.3 * specR + 0.7 * F0) * dot(normal, light)) * sample_ybars[s];
		Z += (sample_intensities[s] * (0.3 * specR + 0.7 * F0) * dot(normal, light)) * sample_zbars[s];
		count += sample_intensities[s];

	}

	X /= count;
	Y /= count;
	Z /= count;

	R = 3.2404542 * X - 1.5371385 * Y - 0.4985314 * Z, G = -0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z, B = 0.0556434 * X - 0.2040259 * Y + 1.0572252 * Z; //  using sRGB values
	clamp(R, 0.0, 1.0);
	clamp(G, 0.0, 1.0);
	clamp(B, 0.0, 1.0);

}

// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, double R, double G, double B)
{
	if (R <= 1.0)
		if (R >= 0.0)
			framebuffer[y][x][0] = R;
		else
			framebuffer[y][x][0] = 0.0;
	else
		framebuffer[y][x][0] = 1.0;
	if (G <= 1.0)
		if (G >= 0.0)
			framebuffer[y][x][1] = G;
		else
			framebuffer[y][x][1] = 0.0;
	else
		framebuffer[y][x][1] = 1.0;
	if (B <= 1.0)
		if (B >= 0.0)
			framebuffer[y][x][2] = B;
		else
			framebuffer[y][x][2] = 0.0;
	else
		framebuffer[y][x][2] = 1.0;
}


void computeImage(void)
{
	int i, j, k;
	double R, G, B;
	Coord3D refpt;
	Coord3D view;
	Coord3D normal;
	Coord3D light;
	Coord3D intpt;
	double xstep = 12.0 / ImageW;
	double ystep = 8.0 / ImageH;
	double t;
	double a, b, c;
	int intsphere;

	refpt.x = -6.0 + xstep / 2.0;
	refpt.y = -4.0 + ystep / 2.0;
	refpt.z = -10.0;

	for (i = 0; i < ImageW; i++, refpt.x += xstep) {
		for (j = 0; j < ImageH; j++, refpt.y += ystep) {
			// Compute the view vector
			view.x = refpt.x; view.y = refpt.y; view.z = refpt.z;
			normalize(view.x, view.y, view.z);

			// Find intersection with sphere (if any) - only 1 sphere can intesect.
			intsphere = -1;
			for (k = 0; (k < 6) && (intsphere == -1); k++) {
				a = 1.0;  // Since normalized;
				b = 2.0 * view.x * (-SphCent[k].x) + 2.0 * view.y * (-SphCent[k].y) + 2.0 * view.z * (-SphCent[k].z);
				c = SphCent[k].x * SphCent[k].x + SphCent[k].y * SphCent[k].y + SphCent[k].z * SphCent[k].z -
					SphRad[k] * SphRad[k];
				if ((b * b - 4 * a * c) >= 0.0) {  // We have an intersection with that sphere
					// Want nearest of two intersections
					t = (-b - sqrt(b * b - 4 * a * c)) / 2.0;
					intsphere = k;
				}
			}

			if (intsphere != -1) { // We had an intersection with a sphere
				intpt.x = t * view.x; intpt.y = t * view.y; intpt.z = t * view.z;
				normal.x = (intpt.x - SphCent[intsphere].x) / SphRad[intsphere];
				normal.y = (intpt.y - SphCent[intsphere].y) / SphRad[intsphere];
				normal.z = (intpt.z - SphCent[intsphere].z) / SphRad[intsphere];
				normalize(normal.x, normal.y, normal.z);

				light.x = Light.x - intpt.x;
				light.y = Light.y - intpt.y;
				light.z = Light.z - intpt.z;
				normalize(light.x, light.y, light.z);
				GetColor(view, normal, light, intsphere, R, G, B);

			}
			else {
				R = G = B = 0.0;
			}
			setFramebuffer(i, j, R, G, B);
		}
		refpt.y = -4.0 + ystep / 2.0;
	}
}

void init(int h = 400, int w = 600)
{
	ImageH = h;
	ImageW = w;

	int i, j;

	// Set up the framebuffer
	framebuffer.resize(ImageH);
	for (i = 0; i < ImageH; i++) {
		framebuffer[i].resize(ImageW);
		for (j = 0; j < ImageW; j++) {
			framebuffer[i][j].resize(3, 0.0);
		}
	}

	// Create Sphere data
	SphCent[0].x = -3.0;
	SphCent[0].y = 1.5;
	SphCent[0].z = -10.0;
	SphCent[0].m = 0.1;
	SphCent[1].x = 0.0;
	SphCent[1].y = 1.5;
	SphCent[1].z = -10.0;
	SphCent[1].m = 0.3;
	SphCent[2].x = 3.0;
	SphCent[2].y = 1.5;
	SphCent[2].z = -10.0;
	SphCent[2].m = 0.5;
	SphCent[3].x = -3.0;
	SphCent[3].y = -1.5;
	SphCent[3].z = -10.0;
	SphCent[3].m = 0.1;
	SphCent[4].x = 0.0;
	SphCent[4].y = -1.5;
	SphCent[4].z = -10.0;
	SphCent[4].m = 0.3;
	SphCent[5].x = 3.0;
	SphCent[5].y = -1.5;
	SphCent[5].z = -10.0;
	SphCent[5].m = 0.5;
	for (i = 0; i < 6; i++) SphRad[i] = 1.0;

	// Set Light Position
	Light.x = 0.0;
	Light.y = 0.0;
	Light.z = 0.0;

	// Eye is at origin, looking down -z axis, y axis is up,
	// Looks at 8x12 window centered around z = -10.
}

int main(int argc, char** argv)
{
	init(400, 600);



	int i = 0;

	////-------- - read wavelength and xbar,ybar,zbar


	inFile.open("ciexyz31.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (!inFile.eof()) {
		getline(inFile, wavelength, ',');
		getline(inFile, xbar, ',');
		getline(inFile, ybar, ',');
		getline(inFile, zbar, '\n');
		try
		{
			wavelengths[i] = stof(wavelength);
			xbars[i] = stof(xbar);
			ybars[i] = stof(ybar);
			zbars[i] = stof(zbar);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}
	}
	inFile.close();

	//-------- - read wavelength and n - Gold


	inFile.open("gold_ior.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ior, '\n');
		try {
			mat_wavelengths[i] = stof(wavelength) * 1000;
			mat_iors[i] = stof(ior);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();

	//-------- - read wavelength and k


	inFile.open("gold_ecoeff.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ecoeff, '\n');
		try {
			mat_ecoeffs[i] = stof(ecoeff);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();

	//10 samples from spectrum

	int min = 400;

	int j;


	for (int s = 0; s < s_max; s++)
	{
		double randNum = min + s * (300 / s_max - 1);
		sample_wavelengths[s] = randNum;

		////color matching functions
		for (j = 0; wavelengths[j] < randNum; j++);
		if (wavelengths[j] == randNum)
		{
			sample_xbars[s] = xbars[j];
			sample_ybars[s] = ybars[j];
			sample_zbars[s] = zbars[j];

		}
		else if (j - 1 > 0)
		{
			double diff = wavelengths[j] - wavelengths[j - 1];
			double t = (randNum - wavelengths[j - 1]) / diff;
			sample_xbars[s] = xbars[j - 1] * (1 - t) + xbars[j] * t;
			sample_ybars[s] = ybars[j - 1] * (1 - t) + ybars[j] * t;
			sample_zbars[s] = zbars[j - 1] * (1 - t) + zbars[j] * t;

		}

		//material properties
		for (j = 0; mat_wavelengths[j] < randNum; j++);
		if (mat_wavelengths[j] == randNum)
		{
			sample_ior[s] = mat_iors[j];
			sample_ecoeff[s] = mat_ecoeffs[j];

		}
		else if (j - 1 > 0)
		{
			double diff = mat_wavelengths[j] - mat_wavelengths[j - 1];
			double t = (randNum - mat_wavelengths[j - 1]) / diff;
			sample_ior[s] = mat_iors[j - 1] * (1 - t) + mat_iors[j] * t;
			sample_ecoeff[s] = mat_ecoeffs[j - 1] * (1 - t) + mat_ecoeffs[j] * t;

		}

	}

	//-------- - read wavelength and n - Chromium


	inFile.open("chromium_ior.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ior, '\n');
		try {
			mat_wavelengths[i] = stof(wavelength) * 1000;
			mat_iors[i] = stof(ior);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();

	//-------- - read wavelength and k


	inFile.open("chromium_ecoeff.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ecoeff, '\n');
		try {
			mat_ecoeffs[i] = stof(ecoeff);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();


	//10 samples from spectrum

	for (int s = 0; s < s_max; s++)
	{
		double randNum = min + s * (300 / s_max - 1);

		//material properties
		for (j = 0; mat_wavelengths[j] < randNum; j++);
		if (mat_wavelengths[j] == randNum)
		{
			chromium_sample_ior[s] = mat_iors[j];
			chromium_sample_ecoeff[s] = mat_ecoeffs[j];

		}
		else if (j - 1 > 0)
		{
			double diff = mat_wavelengths[j] - mat_wavelengths[j - 1];
			double t = (randNum - mat_wavelengths[j - 1]) / diff;
			chromium_sample_ior[s] = mat_iors[j - 1] * (1 - t) + mat_iors[j] * t;
			chromium_sample_ecoeff[s] = mat_ecoeffs[j - 1] * (1 - t) + mat_ecoeffs[j] * t;

		}

	}

	//-------- - read wavelength and n - copper


	inFile.open("copper_ior.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ior, '\n');
		try {
			mat_wavelengths[i] = stof(wavelength) * 1000;
			mat_iors[i] = stof(ior);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();

	//-------- - read wavelength and k


	inFile.open("copper_ecoeff.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ecoeff, '\n');
		try {
			mat_ecoeffs[i] = stof(ecoeff);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();

	for (int s = 0; s < s_max; s++)
	{
		double randNum = min + s * (300 / s_max - 1);

		//material properties
		for (j = 0; mat_wavelengths[j] < randNum; j++);
		if (mat_wavelengths[j] == randNum)
		{
			copper_sample_ior[s] = mat_iors[j];
			copper_sample_ecoeff[s] = mat_ecoeffs[j];

		}
		else if (j - 1 > 0)
		{
			double diff = mat_wavelengths[j] - mat_wavelengths[j - 1];
			double t = (randNum - mat_wavelengths[j - 1]) / diff;
			copper_sample_ior[s] = mat_iors[j - 1] * (1 - t) + mat_iors[j] * t;
			copper_sample_ecoeff[s] = mat_ecoeffs[j - 1] * (1 - t) + mat_ecoeffs[j] * t;

		}

	}

	//-------- - read wavelength and n - Silver


	inFile.open("silver_ior.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ior, '\n');
		try {
			mat_wavelengths[i] = stof(wavelength) * 1000;
			mat_iors[i] = stof(ior);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();

	//-------- - read wavelength and k


	inFile.open("silver_ecoeff.csv");
	if (inFile.is_open()) {
		cout << "File has been opened" << endl;
	}
	else {
		cout << "NO FILE HAS BEEN OPENED" << endl;
	}

	i = 0;
	while (1) {
		getline(inFile, wavelength, ',');
		getline(inFile, ecoeff, '\n');
		try {
			mat_ecoeffs[i] = stof(ecoeff);
			i++;
		}
		catch (const std::exception& e) {
			break;
		}

		if (inFile.eof())
			break;
	}
	inFile.close();


	for (int s = 0; s < s_max; s++)
	{
		double randNum = min + s * (300 / s_max - 1);

		//material properties
		for (j = 0; mat_wavelengths[j] < randNum; j++);
		if (mat_wavelengths[j] == randNum)
		{
			silver_sample_ior[s] = mat_iors[j];
			silver_sample_ecoeff[s] = mat_ecoeffs[j];

		}
		else if (j - 1 > 0)
		{
			double diff = mat_wavelengths[j] - mat_wavelengths[j - 1];
			double t = (randNum - mat_wavelengths[j - 1]) / diff;
			silver_sample_ior[s] = mat_iors[j - 1] * (1 - t) + mat_iors[j] * t;
			silver_sample_ecoeff[s] = mat_ecoeffs[j - 1] * (1 - t) + mat_ecoeffs[j] * t;

		}

	}

	computeImage();
	if (argc > 1) {
		outputBMP(argv[1], framebuffer);
	}
	else {
		outputBMP("Image.bmp", framebuffer);
	}
	return 0;
}
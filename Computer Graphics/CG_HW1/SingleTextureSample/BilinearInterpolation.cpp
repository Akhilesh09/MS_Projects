#include <cstdlib>
#include <iostream>


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

unsigned char* texture_img;
unsigned char* pixmap;

int width1 = 128, height1 = 128;
int width = 275, height = 275, channels1;

//lines bounding polygon
int shape(float x, float y)
{

	double x1 = 75;
	double y1 = 75;
	double n0x1 = cos(70);
	double n0y1 = sin(70);
	double L0x1 = 75 + 25 * n0x1;
	double L0y1 = 75 + 25 * n0y1;

	double x2 = 75;
	double y2 = 75;
	double n0x2 = cos(80);
	double n0y2 = sin(80);
	double L0x2 = 75 + 25 * n0x2;
	double L0y2 = 75 + 25 * n0y2;

	double x3 = 75;
	double y3 = 75;
	double n0x3 = cos(100);
	double n0y3 = sin(100);
	double L0x3 = 75 + 25 * n0x3;
	double L0y3 = 75 + 25 * n0y3;

	double x4 = 75;
	double y4 = 75;
	double n0x4 = cos(110);
	double n0y4 = sin(110);
	double L0x4 = 75 + 25 * n0x4;
	double L0y4 = 75 + 25 * n0y4;

	return(n0x1 * x + n0y1 * y - (L0x1 * n0x1 + L0y1 * n0y1) <= 0 && n0x2 * x + n0y2 * y - (L0x2 * n0x2 + L0y2 * n0y2) <= 0 && n0x3 * x + n0y3 * y - (L0x3 * n0x3 + L0y3 * n0y3) <= 0 && n0x4 * x + n0y4 * y - (L0x4 * n0x4 + L0y4 * n0y4) <= 0);
}


//setup
void setPixels()
{
	//read base 275x275 texture
	stbi_set_flip_vertically_on_load(true);
	texture_img = stbi_load("download.jpg", &width, &height, &channels1, STBI_rgb);

	//create pixmap to store texture mapping result
	pixmap = new unsigned char[width1 * height1 * 3];

	//loop through pixels in image with polygon
	for (int y = 0; y < height1; y++)
	{
		for (int x = 0; x < width1; x++)
		{
			int  k = (y * width1 + x) * 3;

			//check if pixel lower-left corner lies inside polygon
			if (shape(x, y) == 1) {
				//find u,v
				float u = (x) / float(width1);
				float v = (y) / float(height1);

				//find texture corrdinates
				float u_p = u * (width-1);
				float v_p = v * (height-1);

				float Ax=0, Ay=0, Bx=0, By=0, Cx=0, Cy=0, Dx=0 , Dy=0;

				//find x of nearest texels
				for (float m = 0; m < width; m++)
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

				//find y of nearest texels
				for (float n = 0; n < height; n++)
				{
					if (n > v_p)
					{
						Ay = n- 1;
						By = Ay;
						Cy = n;
						Dy = Cy;
						break;
					}
				}
				
				//interpolate between AB and CD...then interpolate between those 2 points
				pixmap[k]     = (Cy - v_p) * (texture_img[int(Ay*width+Ax)*3] * (Bx - u_p) +			texture_img[int(By * width + Bx) * 3] * (u_p - Ax) ) + (v_p - Ay)*(	texture_img[int(Cy * width + Cx) * 3] * (Bx - u_p) +		texture_img[int(Dy * width + Dx) * 3] * (u_p - Ax) );
				pixmap[k + 1] = (Cy - v_p) * (texture_img[int(Ay * width + Ax) * 3+1] * (Bx - u_p) + texture_img[int(By * width + Bx) * 3+1] * (u_p - Ax)) + (v_p - Ay) * (texture_img[int(Cy * width + Cx) * 3+1] * (Bx - u_p) + texture_img[int(Dy * width + Dx) * 3+1] * (u_p - Ax));
				pixmap[k + 2] = (Cy - v_p) * (texture_img[int(Ay * width + Ax) * 3+2] * (Bx - u_p) + texture_img[int(By * width + Bx) * 3+2] * (u_p - Ax)) + (v_p - Ay) * (texture_img[int(Cy * width + Cx) * 3+2] * (Bx - u_p) + texture_img[int(Dy * width + Dx) * 3+2] * (u_p - Ax));
			}
			else
			{
				//background
				pixmap[k] = 0;
				pixmap[k + 1] = 0;
				pixmap[k + 2] = 0;
			}


		}
	}
	//save result
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("bilinear_texmap.jpg", width1, height1, 3, pixmap, 100);
}

// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char* argv[])
{

	//initialize the global variables
	width = 275;
	height = 275;

	setPixels();

	return 0; 
}

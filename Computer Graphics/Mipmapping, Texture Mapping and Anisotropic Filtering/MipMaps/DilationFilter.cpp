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

int width = 256, height = 256, channels;

//to find brighter color
int max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

//setup
void setPixels()
{
	//read base 256x256 texture
	stbi_set_flip_vertically_on_load(true);
	texture_img = stbi_load("cat.jpg", &width, &height, &channels, STBI_rgb);

	//rename and save base texture as base mipmap
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(("Dilation" + std::to_string(width) + "by" + std::to_string(height) + ".jpg").c_str(), width, height, 3, texture_img, 100);

	//reducing by a power of 2 each time
	for (int size = 2; width / size >= 1; size *= 2)
	{
		//create pixmap of resulting mipmap dimension
		pixmap = new unsigned char[width / size * height / size * 3];

		//loop through pixels in resulting pixmap
		for (int y = 0; y < height / size; y++)
		{
			for (int x = 0; x < width / size; x++)
			{
				int r = 0, g = 0, b = 0;
				int dest = ((y)*width / size + (x)) * 3;

				//determine mapping from pixel to grid and find brightest color
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						r = max(r, texture_img[((size * y + j) * width + (size * x + i)) * 3]);
						g = max(g, texture_img[((size * y + j) * width + (size * x + i)) * 3 + 1]);
						b = max(b, texture_img[((size * y + j) * width + (size * x + i)) * 3 + 2]);

					}
				}
				//assign brightest color
				pixmap[dest] = r;
				pixmap[dest + 1] = g;
				pixmap[dest + 2] = b;
			}
		}
		//save mipmap
		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(("Dilation" + std::to_string(width / size) + "by" + std::to_string(width / size) + ".jpg").c_str(), width / size, height / size, 3, pixmap, 100);
	}

}

// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char* argv[])
{

	//initialize the global variables
	width = 256;
	height = 256;

	setPixels();

	return 0; //This line never gets reached. We use it because "main" is T_yPr int.
}

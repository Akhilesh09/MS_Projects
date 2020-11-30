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

//setup
void setPixels()
{
	//read base 256x256 texture
	stbi_set_flip_vertically_on_load(true);
	texture_img = stbi_load("texture.jpg", &width, &height, &channels, STBI_rgb);

	//rename and save base texture as base mipmap
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(("BoxCircle" + std::to_string(width) + "by" + std::to_string(height) + ".jpg").c_str(), width, height, 3, texture_img, 100);

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
				double r = 0, g = 0, b = 0;
				int dest = ((y)*width / size + (x)) * 3;
				int count = 0;
				//determine center of grid mapped at pixel
				float center_y = (size * y + (size / 2));
				float center_x = (size * x + (size / 2 - 1));
				//determine mapping from pixel to grid and add colors
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						//check if pixel center lies inside circle
						//if yes, add pixel color
						if (pow((size * x + i + 0.5) - center_x, 2) + pow((size * y + j + 0.5) - center_y, 2) <= pow(size / 2, 2))
						{
							r += texture_img[((size * y + j) * width + (size * x + i)) * 3];
							g += texture_img[((size * y + j) * width + (size * x + i)) * 3 + 1];
							b += texture_img[((size * y + j) * width + (size * x + i)) * 3 + 2];
							count++;
						}

					}
				}
				//average colors by number of pixels inside circle
				pixmap[dest] = r / count;
				pixmap[dest + 1] = g / count;
				pixmap[dest + 2] = b / count;
			}
		}
		//save mipmap
		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(("BoxCircle" + std::to_string(width / size) + "by" + std::to_string(height / size) + ".jpg").c_str(), width / size, height / size, 3, pixmap, 100);
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

	return 0;
}

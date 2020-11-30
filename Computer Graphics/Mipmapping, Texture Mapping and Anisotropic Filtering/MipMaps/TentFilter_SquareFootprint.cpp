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

//euclidean distance between 2 points
float dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

//setup
void setPixels()
{
	//read base 256x256 texture
	stbi_set_flip_vertically_on_load(true);
	texture_img = stbi_load("cat.jpg", &width, &height, &channels, STBI_rgb);

	//rename and save base texture as base mipmap
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(("TentSquare" + std::to_string(width) + "by" + std::to_string(height) + ".jpg").c_str(), width, height, 3, texture_img, 100);

	int size;
	// to store distances from grid center
	float dists[256][256];

	//reducing by a power of 2 each time
	for (size = 2; width / size >= 1; size *= 2)
	{
		//create pixmap of resulting mipmap dimension
		pixmap = new unsigned char[width / size * height / size * 3];

		//loop through pixels in resulting pixmap
		for (int y = 0; y < height / size; y++)
		{
			for (int x = 0; x < width / size; x++)
			{
				float r = 0, g = 0, b = 0;
				int dest = ((y)*width / size + (x)) * 3;

				//determine center of grid mapped at pixel
				float center_y = (size * y + (size / 2));
				float center_x = (size * x + (size / 2 - 1));

				float total = 0;

				//determine mapping from pixel to grid and find distances from grid center
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						dists[i][j] = dist((size * x + i + 0.5), (size * y + j + 0.5), center_x, center_y);
						total += dists[i][j];

					}
				}

				//determine mapping from pixel to grid and add distance-weighted colors
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						r += texture_img[((size * y + j) * width + (size * x + i)) * 3] * (1 - dists[i][j] / total);
						g += texture_img[((size * y + j) * width + (size * x + i)) * 3 + 1] * (1 - dists[i][j] / total);
						b += texture_img[((size * y + j) * width + (size * x + i)) * 3 + 2] * (1 - dists[i][j] / total);

					}
				}

				//average colors by number of pixels in grid
				pixmap[dest] = r / pow(size, 2);
				pixmap[dest + 1] = g / pow(size, 2);;
				pixmap[dest + 2] = b / pow(size, 2);;
			}
		}
		//save mipmap
		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(("TentSquare" + std::to_string(width / size) + "by" + std::to_string(height / size) + ".jpg").c_str(), width / size, height / size, 3, pixmap, 100);
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

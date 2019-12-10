#include <iostream>
#include <fstream>

using namespace std;

//function to open image file for writing

void write(char fname[], char data[], int size) {
	FILE *pimage = fopen(fname, "wb");
	fwrite(data, 1, size, pimage);
	fclose(pimage);

}

int main()
{
	
	//writing .ppm file
	
	ofstream img("img1.ppm");
	img << "P3" << endl;
	img << 300 << endl;
	img << 300 << endl;
	img << 255 << endl;
	for (int i = 0; i < 300; i++) {
		for (int j = 0; j < 300; j++) {
			int r = j % 255;
			int g = i % 255;
			int b = (i * j) % 255;
			img << r << " " << g << " " << b << " "; 
		}
		img<< endl;
	}
	system("open img1.ppm");
	return 0;
}

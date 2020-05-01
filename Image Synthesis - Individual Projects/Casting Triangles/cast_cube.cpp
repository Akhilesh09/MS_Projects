#include <cstdlib>
#include <iostream>
#include <GL/glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include </home/user/Desktop/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include </home/user/Desktop/stb_image_write.h>
#include </home/user/Desktop/stb_include.h>
#include <string>
#include<cmath>
#include<math.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================

unsigned char *result;

int width = 200, height = 200, channels1, channels2, channels3;

class Vector
{
public:
	float x,y,z;
friend Vector operator-(Vector a, Vector b)
{
	Vector res;
	res.x=a.x-b.x;
	res.y=a.y-b.y;
	res.z=a.z-b.z;
	return res;
}
friend Vector operator+(Vector a, Vector b)
{
	Vector res;
	res.x=a.x+b.x;
	res.y=a.y+b.y;
	res.z=a.z+b.z;
	return res;
}
friend float operator*(Vector a, Vector b)
{
	
	return a.x*b.x+a.y*b.y+a.z*b.z;
}
friend Vector operator*(Vector a, float b)
{
	Vector res;
	res.x=a.x*b;
	res.y=a.y*b;
	res.z=a.z*b;
	return res;
}

friend ostream& operator<<(ostream& os, Vector& v)
{
	os<<"("<<v.x<<","<<v.y<<","<<v.z<<")";
	return os;
}


};

float r=50;


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
	res.x=a.y*b.z-b.y*a.z;
	res.y=a.z*b.x-b.z*a.x;
	res.z=a.x*b.y-b.x*a.y;
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


//setup
void setPixels()
{
	//camera setup
	//camera up vector
	Vector Vup;
	Vup.x=1;
	Vup.y=0;
	Vup.z=0;
	
	//camera view direction
	Vector V_view;
	V_view.x=0;
	V_view.y=-1;
	V_view.z=1;

	//camera local normals
	Vector V0= cross_product(V_view,Vup);
	Vector n0;
	n0=V0*(1/magnitude(V0.x,V0.y,V0.z));
	Vector n2;
	n2=V_view*(1/magnitude(V_view.x,V_view.y,V_view.z));

	Vector n1= cross_product(n0,n2);

	//eyepoint
	Vector Pe;
	Pe.x=8;
	Pe.y=15;
	Pe.z=-15;

	//camera dimensions and distance from eyepoint
	float d=100,sx=200;
	float sy=sx*height/width;

	//center of camera
	Vector P_Cam;
	P_Cam=Pe+(n2*d);

	//bottom-left corner of camera
	Vector P00;
	P00=P_Cam-(n0*(sx/2))- (n1*(sy/2));

	//triangle vertices
	Vector T_P0,T_P1,T_P2;

	T_P0.x=5;
	T_P0.y=-5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=-5;

	T_P2.x=5;
	T_P2.y=-5;
	T_P2.z=5;

	//edge normals
	Vector T_V0,T_V1,T_V2;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	//area and face normal
	Vector A,T_n;
	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;
	
			//point on camera plane
			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));
			
			//primary ray direction
			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;



			//check if hitpoint lies on triangle
			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));

				//barycentric coordinates
				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+0*(T);
					result[i+2] = 0*(1-T)+255*(T);
				}
			}
		}
	}


	T_P0.x=5;
	T_P0.y=-5;
	T_P0.z=5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=-5;

	T_P2.x=-5;
	T_P2.y=-5;
	T_P2.z=5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+0*(T);
					result[i+2] = 0*(1-T)+255*(T);
				}
			}
		}
	}




	T_P0.x=5;
	T_P0.y=-5;
	T_P0.z=5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=5;

	T_P2.x=5;
	T_P2.y=5;
	T_P2.z=5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+0*(T);
					result[i+2] = 0*(1-T)+255*(T);
				}
			}
		}
	}


	T_P0.x=5;
	T_P0.y=5;
	T_P0.z=5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=5;

	T_P2.x=-5;
	T_P2.y=5;
	T_P2.z=5;


	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);


				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+0*(T);
					result[i+2] = 0*(1-T)+255*(T);
				}
			}
		}
	}


	//top and bottom

	T_P0.x=5;
	T_P0.y=5;
	T_P0.z=-5;

	T_P1.x=5;
	T_P1.y=5;
	T_P1.z=5;

	T_P2.x=5;
	T_P2.y=-5;
	T_P2.z=5;


	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 255*(1-T)+0*(T);
					result[i+2] = 255*(1-T)+255*(T);
				}
			}
		}
	}

	T_P0.x=5;
	T_P0.y=5;
	T_P0.z=-5;

	T_P1.x=5;
	T_P1.y=-5;
	T_P1.z=5;

	T_P2.x=5;
	T_P2.y=-5;
	T_P2.z=-5;


	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;

			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 255*(1-T)+0*(T);
					result[i+2] = 255*(1-T)+255*(T);
				}
			}
		}
	}

	T_P0.x=-5;
	T_P0.y=-5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=5;

	T_P2.x=-5;
	T_P2.y=5;
	T_P2.z=5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 255*(1-T)+0*(T);
					result[i+2] = 255*(1-T)+255*(T);
				}
			}
		}
	}

	T_P0.x=-5;
	T_P0.y=-5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=5;
	T_P1.z=5;

	T_P2.x=-5;
	T_P2.y=5;
	T_P2.z=-5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 255*(1-T)+0*(T);
					result[i+1] = 255*(1-T)+0*(T);
					result[i+2] = 255*(1-T)+255*(T);
				}
			}
		}
	}


	T_P0.x=5;
	T_P0.y=-5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=-5;

	T_P2.x=5;
	T_P2.y=5;
	T_P2.z=-5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{
					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 0*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+255*(T);
					result[i+2] = 255*(1-T)+0*(T);
				}
			}
		}
	}

	T_P0.x=5;
	T_P0.y=5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=-5;
	T_P1.z=-5;

	T_P2.x=-5;
	T_P2.y=5;
	T_P2.z=-5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{
					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 0*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+255*(T);
					result[i+2] = 255*(1-T)+0*(T);
				}
			}
		}
	}

	T_P0.x=5;
	T_P0.y=5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=5;
	T_P1.z=-5;

	T_P2.x=-5;
	T_P2.y=5;
	T_P2.z=5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 0*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+255*(T);
					result[i+2] = 255*(1-T)+0*(T);
				}
			}
		}
	}

	T_P0.x=5;
	T_P0.y=5;
	T_P0.z=-5;

	T_P1.x=-5;
	T_P1.y=5;
	T_P1.z=5;

	T_P2.x=5;
	T_P2.y=5;
	T_P2.z=5;

	T_V0=T_P0-T_P2;
	T_V1=T_P1-T_P0;
	T_V2=T_P2-T_P1;

	A=cross_product(T_V0,T_V1);
	A=A*0.5;
	T_n=A*(1/magnitude(A.x,A.y,A.z));


	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int i = (y * width + x) * 3;
				
			Vector Pp,npe,P_hit;

			Pp=P00+(n0*(sx*x/width))+(n1*(sy*y/height));

			npe=Pp-Pe;
			npe=npe*(1/magnitude(npe.x,npe.y,npe.z));

			Vector T_A0,T_A1,T_A2;
			Vector T_n0,T_n1,T_n2;




			float num=(T_n*(T_P1-Pe));

			float den=(T_n*npe);

			if(num<0 && den<0)
			{
				float t_hit=num/den;

				P_hit=Pe+(npe*t_hit);

				T_A0=cross_product(P_hit-T_P2,T_P1-P_hit);
				T_A0=T_A0*0.5;
				T_A1=cross_product(P_hit-T_P0,T_P2-P_hit);
				T_A1=T_A1*0.5;
				T_A2=cross_product(P_hit-T_P1,T_P0-P_hit);
				T_A2=T_A2*0.5;

				T_n0=T_A0*(1/magnitude(T_A0.x,T_A0.y,T_A0.z));

				T_n1=T_A1*(1/magnitude(T_A1.x,T_A1.y,T_A1.z));

				T_n2=T_A2*(1/magnitude(T_A2.x,T_A2.y,T_A2.z));


				float s,t,u;
				s=(T_n*T_A1)/magnitude(A.x,A.y,A.z);
				t=(T_n*T_A2)/magnitude(A.x,A.y,A.z);
				u=(T_n*T_A0)/magnitude(A.x,A.y,A.z);

				if(s>0 && s<1 && t>0 && t<1 && u>0 && u<1)
				{

					Vector L;
					L.x=(20);
					L.y=(0);
					L.z=(-1);
					L=L*(1/magnitude(L.x,L.y,L.z));
					double T = 0.5*(L.x*T_n.x+ L.y*T_n.y+L.z*T_n.z) + 0.5;
					T = crop(0, 1, T);


					result[i] = 0*(1-T)+0*(T);
					result[i+1] = 0*(1-T)+0*(T);
					result[i+2] = 255*(1-T)+255*(T);
				}
			}
		}
	}
			
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE,result);
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
int main(int argc, char *argv[])
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

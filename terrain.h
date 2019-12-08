#ifndef TERRAIN_H
#define TERRAIN_H

#include <math.h>
#include "vec3f.h"

class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;
			
			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}
			
			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}
			
			computedNormals = false;
		}
		
		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;
			
			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}
		
		int width() {
			return w;
		}
		
		int length() {
			return l;
		}
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}
		
		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}
			
			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}
			
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);
					
					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f lft;
					if (x > 0) {
						lft = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f rgt;
					if (x < w - 1) {
						rgt = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}
					
					if (x > 0 && z > 0) {
						sum += out.cross(lft).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += lft.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(rgt).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += rgt.cross(out).normalize();
					}
					
					normals2[z][x] = sum;
				}
			}
			
			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];
					
					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}
					
					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}
			
			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;
			
			computedNormals = true;
		}
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

Terrain* loadTerrain(const char* filename, float height) {
	int req_channels = 3; // 3 color channels of BMP-file   
	int data_width = 0, data_height = 0, channels = 0;
	unsigned char *data = stbi_load(filename, &data_width, &data_height, &channels, req_channels);
	
	Terrain* t = new Terrain(data_width, data_height);
	for(int y = 0; y < data_height; y++) {
		for(int x = 0; x < data_width; x++) {
			unsigned char color = (unsigned char)data[3 * (y * data_width + x)];
			float h = height * ((color / 32.0f));
			t->setHeight(x, y, h);
		}
	}
	stbi_image_free(data);
	t->computeNormals();
	return t;
}

float heightAt(Terrain* terrain, float x, float z) {
	//Returns the approximate height of the terrain at the specified (x, z) position
	//Make (x, z) lie within the bounds of the terrain
	if (x < 0) {
		x = 0;
	}
	else if (x > terrain->width() - 1) {
		x = terrain->width() - 1;
	}
	if (z < 0) {
		z = 0;
	}
	else if (z > terrain->length() - 1) {
		z = terrain->length() - 1;
	}
	
	//Compute the grid cell in which (x, z) lies and how close we are to the
	//left and outward edges
	int leftX = (int)x;
	if (leftX == terrain->width() - 1) {
		leftX--;
	}
	float fracX = x - leftX;
	
	int outZ = (int)z;
	if (outZ == terrain->width() - 1) {
		outZ--;
	}
	float fracZ = z - outZ;
	
	//Compute the four heights for the grid cell
	float h11 = terrain->getHeight(leftX, outZ);
	float h12 = terrain->getHeight(leftX, outZ + 1);
	float h21 = terrain->getHeight(leftX + 1, outZ);
	float h22 = terrain->getHeight(leftX + 1, outZ + 1);
	
	//Take a weighted average of the four heights
	return (1 - fracX) * ((1 - fracZ) * h11 + fracZ * h12) +
		fracX * ((1 - fracZ) * h21 + fracZ * h22);
}

#endif

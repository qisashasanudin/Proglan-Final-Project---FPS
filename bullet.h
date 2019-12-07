#ifndef ENDERMAN_H
#define ENDERMAN_H

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include "main.h"
#include "imageloader.h"
#include "md2model.h"
#include "vec3f.h"
#include "text3d.h"
#include "terrain.h"
#include "enderman.h"

using namespace std;

const float PI = 3.1415926535f;
const int NUM_BulletS = 0;
//The amount of time between each time that we handle collisions
const float TIME_BETWEEN_HANDLE_COLLISIONS = 0.01f;
//The amount by which the Bullet class's step function advances the state of a Bullet
const float Bullet_STEP_TIME = 0.01f;
float terrain_angle = 60.0f;
//The amount of time until we next check for and handle all collisions
float _timeUntilHandleCollisions = 0;
//The total number of collisions that have occurred
int _numCollisions;
float _angle = 0;

//Represents a Bullet
class Bullet {
	private:
		MD2Model* model;
		float x0;
		float z0;
		float y0;
		float radius0; //The approximate radius of the Bullet
		float speed;
		//The angle at which the Bullet is currently walking, in radians.  An angle
		//of 0 indicates the positive x direction, while an angle of PI / 2
		//indicates the positive z direction.  The angle always lies between 0
		//and 2 * PI.
		float angle;
		//The amount of time until step() should next be called
		float timeUntilNextStep;
		
		//Advances the state of the Bullet by Bullet_STEP_TIME seconds (without
		//altering animTime)
		void step() {			
			//Update the position and angle
			float maxX = (terrain->width() - 1) - radius0;
			float maxZ = (terrain->length() - 1) - radius0;
			
			x0 += velocityX() * Bullet_STEP_TIME;
			z0 += velocityZ() * Bullet_STEP_TIME;
			bool hitEnv = false;
			if (y0 < y()) {
				y0 = y();
				hitEnv = true;
			}
			if (x0 < radius0) {
				x0 = radius0;
				hitEnv = true;
			}
			else if (x0 > maxX) {
				x0 = maxX;
				hitEnv = true;
			}
			if (z0 < radius0) {
				z0 = radius0;
				hitEnv = true;
			}
			else if (z0 > maxZ) {
				z0 = maxZ;
				hitEnv = true;
			}
			
			if (hitEnv) {
				//Destroy Bullet
			}
			
			angle += 0.05f * speed * Bullet_STEP_TIME;
			
			while (angle > 2 * PI) {
				angle -= 2 * PI;
			}
			while (angle < 0) {
				angle += 2 * PI;
			}
		}
	public:
		Bullet(MD2Model* model1, float player_x, float player_y, float player_ly) {
			model = model1;
			timeUntilNextStep = 0;
			//Initialize certain fields to random values
			radius0 = 0.25f;
			x0 = player_x;
			z0 = player_y;
			speed = 10;
			angle = player_ly;
		}

		void advance(float dt) {
			if (dt > 0) {
				dt -= Bullet_STEP_TIME;
			}else{
				step();
			}
		}
		
		void draw() {
			if (model == NULL) {
				return;
			}
			
			float scale = radius0 ;
			
			glPushMatrix();
			glTranslatef(x0, scale * 10.0f + y(), z0);
			glRotatef(90 - angle * 180 / PI, 0, 1, 0);
			glColor3f(1, 1, 1);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glScalef(scale, scale, scale);
			model->draw(0);
			glPopMatrix();
		}
		
		float x() {
			return x0;
		}
		
		float z() {
			return z0;
		}
		
		//Returns the current height of the Bullet on the terrain
		float y() {
			return terrainScale *heightAt(terrain, x0 / terrainScale, z0 / terrainScale);
		}
		
		float velocityX() {
			return speed * cos(angle);
		}
		
		float velocityZ() {
			return speed * sin(angle);
		}
		
		//Returns the approximate radius of the Bullet
		float radius() {
			return radius0;
		}
		
		//Returns the angle at which the Bullet is currently walking, in radians.
		//An angle of 0 indicates the positive x direction, while an angle of
		//PI / 2 indicates the positive z direction.
		float walkAngle() {
			return angle;
		}	
		
		//Adjusts the angle at which this Bullet is walking in response to a
		//collision with the specified Bullet
		void bounceOff(Bullet* otherBullet) {
			float vx = velocityX();
			float vz = velocityZ();
			
			float dx = otherBullet->x0 - x0;
			float dz = otherBullet->z0 - z0;
			float m = sqrt(dx * dx + dz * dz);
			dx /= m;
			dz /= m;
			
			float dotProduct = vx * dx + vz * dz;
			vx -= 2 * dotProduct * dx;
			vz -= 2 * dotProduct * dz;
			
			if (vx != 0 || vz != 0) {
				angle = atan2(vz, vx);
			}
		}
};








void key_calc(float terrainScale){
	x += speed_walk * lx;
	z += speed_walk * lz;
	y += speed_walk * ly;

}







struct BulletPair {
	Bullet* Bullet1;
	Bullet* Bullet2;
};

const int MAX_QUADTREE_DEPTH = 6;
const int MIN_BulletS_PER_QUADTREE = 2;
const int MAX_BulletS_PER_QUADTREE = 5;

//Our data structure for making collision detection faster
class Quadtree {
	private:
		float minX;
		float minZ;
		float maxX;
		float maxZ;
		float centerX; //(minX + maxX) / 2
		float centerZ; //(minZ + maxZ) / 2
		
		/* The children of this, if this has any.  children[0][*] are the
		 * children with x coordinates ranging from minX to centerX.
		 * children[1][*] are the children with x coordinates ranging from
		 * centerX to maxX.  Similarly for the other dimension of the children
		 * array.
		 */
		Quadtree *children[2][2];
		//Whether this has children
		bool hasChildren;
		//The Bullets in this, if this doesn't have any children
		set<Bullet*> Bullets;
		//The depth of this in the tree
		int depth;
		//The number of Bullets in this, including those stored in its children
		int numBullets;
		
		//Adds a Bullet to or removes one from the children of this
		void fileBullet(Bullet* Bullet, float x, float z, bool addBullet) {
			//Figure out in which child(ren) the Bullet belongs
			for(int xi = 0; xi < 2; xi++) {
				if (xi == 0) {
					if (x - Bullet->radius() > centerX) {
						continue;
					}
				}
				else if (x + Bullet->radius() < centerX) {
					continue;
				}
				
				for(int zi = 0; zi < 2; zi++) {
					if (zi == 0) {
						if (z - Bullet->radius() > centerZ) {
							continue;
						}
					}
					else if (z + Bullet->radius() < centerZ) {
						continue;
					}
					
					//Add or remove the Bullet
					if (addBullet) {
						children[xi][zi]->add(Bullet);
					}
					else {
						children[xi][zi]->remove(Bullet, x, z);
					}
				}
			}
		}
		
		//Creates children of this, and moves the Bullets in this to the children
		void haveChildren() {
			for(int x = 0; x < 2; x++) {
				float minX2;
				float maxX2;
				if (x == 0) {
					minX2 = minX;
					maxX2 = centerX;
				}
				else {
					minX2 = centerX;
					maxX2 = maxX;
				}
				
				for(int z = 0; z < 2; z++) {
					float minZ2;
					float maxZ2;
					if (z == 0) {
						minZ2 = minZ;
						maxZ2 = centerZ;
					}
					else {
						minZ2 = centerZ;
						maxZ2 = maxZ;
					}
					
					children[x][z] =
						new Quadtree(minX2, maxX2, minZ2, maxZ2, depth + 1);
				}
			}
			
			//Remove all Bullets from "Bullets" and add them to the new children
			for(set<Bullet*>::iterator it = Bullets.begin(); it != Bullets.end();
					it++) {
				Bullet* Bullet = *it;
				fileBullet(Bullet, Bullet->x(), Bullet->z(), true);
			}
			Bullets.clear();
			
			hasChildren = true;
		}
		
		//Adds all Bullets in this or one of its descendants to the specified set
		void collectBullets(set<Bullet*> &gs) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int z = 0; z < 2; z++) {
						children[x][z]->collectBullets(gs);
					}
				}
			}
			else {
				for(set<Bullet*>::iterator it = Bullets.begin(); it != Bullets.end();
						it++) {
					Bullet* Bullet = *it;
					gs.insert(Bullet);
				}
			}
		}
		
		//Destroys the children of this, and moves all Bullets in its descendants
		//to the "Bullets" set
		void destroyChildren() {
			//Move all Bullets in descendants of this to the "Bullets" set
			collectBullets(Bullets);
			
			for(int x = 0; x < 2; x++) {
				for(int z = 0; z < 2; z++) {
					delete children[x][z];
				}
			}
			
			hasChildren = false;
		}
		
		//Removes the specified Bullet at the indicated position
		void remove(Bullet* Bullet, float x, float z) {
			numBullets--;
			
			if (hasChildren && numBullets < MIN_BulletS_PER_QUADTREE) {
				destroyChildren();
			}
			
			if (hasChildren) {
				fileBullet(Bullet, x, z, false);
			}
			else {
				Bullets.erase(Bullet);
			}
		}
	public:
		//Constructs a new Quadtree.  d is the depth, which starts at 1.
		Quadtree(float minX1, float minZ1, float maxX1, float maxZ1, int d) {
			minX = minX1;
			minZ = minZ1;
			maxX = maxX1;
			maxZ = maxZ1;
			centerX = (minX + maxX) / 2;
			centerZ = (minZ + maxZ) / 2;
			
			depth = d;
			numBullets = 0;
			hasChildren = false;
		}
		
		~Quadtree() {
			if (hasChildren) {
				destroyChildren();
			}
		}
		
		//Adds a Bullet to this
		void add(Bullet* Bullet) {
			numBullets++;
			if (!hasChildren && depth < MAX_QUADTREE_DEPTH &&
				numBullets > MAX_BulletS_PER_QUADTREE) {
				haveChildren();
			}
			
			if (hasChildren) {
				fileBullet(Bullet, Bullet->x(), Bullet->z(), true);
			}
			else {
				Bullets.insert(Bullet);
			}
		}
		
		//Removes a Bullet from this
		void remove(Bullet* Bullet) {
			remove(Bullet, Bullet->x(), Bullet->z());
		}
		
		//Changes the position of a Bullet in this from the specified position to
		//its current position
		void BulletMoved(Bullet* Bullet, float x, float z) {
			remove(Bullet, x, z);
			add(Bullet);
		}
		
		//Adds potential collisions to the specified set
		void potentialCollisions(vector<BulletPair> &collisions) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int z = 0; z < 2; z++) {
						children[x][z]->potentialCollisions(collisions);
					}
				}
			}
			else {
				//Add all pairs (Bullet1, Bullet2) from Bullets
				for(set<Bullet*>::iterator it = Bullets.begin(); it != Bullets.end();
						it++) {
					Bullet* Bullet1 = *it;
					for(set<Bullet*>::iterator it2 = Bullets.begin();
							it2 != Bullets.end(); it2++) {
						Bullet* Bullet2 = *it2;
						//This test makes sure that we only add each pair once
						if (Bullet1 < Bullet2) {
							BulletPair gp;
							gp.Bullet1 = Bullet1;
							gp.Bullet2 = Bullet2;
							collisions.push_back(gp);
						}
					}
				}
			}
		}
};

void potentialCollisions(vector<BulletPair> &cs, Quadtree* quadtree) {
	quadtree->potentialCollisions(cs);
}

//Returns whether Bullet1 and Bullet2 are currently colliding
bool testCollision(Bullet* Bullet1, Bullet* Bullet2) {
	float dx = Bullet1->x() - Bullet2->x();
	float dz = Bullet1->z() - Bullet2->z();
	float r = Bullet1->radius() + Bullet2->radius();
	if (dx * dx + dz * dz < r * r) {
		float vx = Bullet1->velocityX() - Bullet2->velocityX();
		float vz = Bullet1->velocityZ() - Bullet2->velocityZ();
		return vx * dx + vz * dz < 0;
	}
	else {
		return false;
	}
}

void handleCollisions(vector<Bullet*> &Bullets, Quadtree* quadtree, int &numCollisions) {
	vector<BulletPair> gps;
	potentialCollisions(gps, quadtree);
	for(unsigned int i = 0; i < gps.size(); i++) {
		BulletPair gp = gps[i];
		
		Bullet* g1 = gp.Bullet1;
		Bullet* g2 = gp.Bullet2;
		if (testCollision(g1, g2)) {
			g1->bounceOff(g2);
			g2->bounceOff(g1);
			numCollisions++;
		}
	}
}

//Moves the Bullets over the given interval of time, without handling collisions
void moveBullets(vector<Bullet*> &Bullets, Quadtree* quadtree, float dt) {
	for(unsigned int i = 0; i < Bullets.size(); i++) {
		Bullet* Bullet = Bullets[i];
		float oldX = Bullet->x();
		float oldZ = Bullet->z();
		Bullet->advance(dt);
		quadtree->BulletMoved(Bullet, oldX, oldZ);
	}
}

//Advances the state of the Bullets over the indicated interval of time
void advance(vector<Bullet*> &Bullets, Quadtree* quadtree, float t, float &timeUntilHandleCollisions, int &numCollisions) {
	while (t > 0) {
		if (timeUntilHandleCollisions <= t) {
			moveBullets(Bullets, quadtree, timeUntilHandleCollisions);
			handleCollisions(Bullets, quadtree, numCollisions);
			t -= timeUntilHandleCollisions;
			timeUntilHandleCollisions = TIME_BETWEEN_HANDLE_COLLISIONS;
		}
		else {
			moveBullets(Bullets, quadtree, t);
			timeUntilHandleCollisions -= t;
			t = 0;
		}
	}
}

//Returns a vector of numBullets new Bullets
vector<Bullet*> makeBullets(int numBullets, MD2Model* model, Terrain* terrain) {
	vector<Bullet*> Bullets;
	for(int i = 0; i < numBullets; i++) {
		Bullets.push_back(new Bullet(model,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1)));
	}
	return Bullets;
}

MD2Model* _model;
vector<Bullet*> _Bullets;
Quadtree* _quadtree;

void update(int value) {
	_angle += 0.3f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	advance(_Bullets,
			_quadtree,
			0.025f,
			_timeUntilHandleCollisions,
			_numCollisions);
	
	glutPostRedisplay();
	glutTimerFunc(1000/60, update, 0);
}

#endif

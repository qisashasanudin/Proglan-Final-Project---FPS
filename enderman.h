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

#include "imageloader.h"
#include "md2model.h"
#include "vec3f.h"
#include "text3d.h"
#include "terrain.h"
//#include "bullet.h"

using namespace std;

const float PI = 3.1415926535f;
const int NUM_EndermanS = 500;
//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 10000;
//The amount of time between each time that we handle collisions
const float TIME_BETWEEN_HANDLE_COLLISIONS = 0.01f;
//The amount by which the Enderman class's step function advances the state of a Enderman
const float Enderman_STEP_TIME = 0.01f;
float terrain_angle = 60.0f;
//The amount of time until we next check for and handle all collisions
float _timeUntilHandleCollisions = 0;
//The total number of collisions that have occurred
int _numCollisions;
float _angle = 0;

float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

//Represents a Enderman
class Enderman {
	private:
		MD2Model* model;
		Terrain* terrain;
		float terrainScale; //The scaling factor for the terrain
		float x0;
		float z0;
		float animTime; //The current position in the animation of the model
		float radius0; //The approximate radius of the Enderman
		float speed;
		//The angle at which the Enderman is currently walking, in radians.  An angle
		//of 0 indicates the positive x direction, while an angle of PI / 2
		//indicates the positive z direction.  The angle always lies between 0
		//and 2 * PI.
		float angle;
		//The amount of time until step() should next be called
		float timeUntilNextStep;
		bool isTurningLeft; //Whether the Enderman is currently turning left
		float timeUntilSwitchDir; //The amount of time until switching direction
		
		//Advances the state of the Enderman by Enderman_STEP_TIME seconds (without
		//altering animTime)
		void step() {
			//Update the turning direction information
			timeUntilSwitchDir -= Enderman_STEP_TIME;
			while (timeUntilSwitchDir <= 0) {
				timeUntilSwitchDir += 20 * randomFloat() + 15;
				isTurningLeft = !isTurningLeft;
			}
			
			//Update the position and angle
			float maxX = terrainScale * (terrain->width() - 1) - radius0;
			float maxZ = terrainScale * (terrain->length() - 1) - radius0;
			
			x0 += velocityX() * Enderman_STEP_TIME;
			z0 += velocityZ() * Enderman_STEP_TIME;
			bool hitEdge = false;
			if (x0 < radius0) {
				x0 = radius0;
				hitEdge = true;
			}
			else if (x0 > maxX) {
				x0 = maxX;
				hitEdge = true;
			}
			
			if (z0 < radius0) {
				z0 = radius0;
				hitEdge = true;
			}
			else if (z0 > maxZ) {
				z0 = maxZ;
				hitEdge = true;
			}
			
			if (hitEdge) {
				//Turn more quickly if we've hit the edge
				if (isTurningLeft) {
					angle -= 0.5f * speed * Enderman_STEP_TIME;
				}
				else {
					angle += 0.5f * speed * Enderman_STEP_TIME;
				}
			}
			else if (isTurningLeft) {
				angle -= 0.05f * speed * Enderman_STEP_TIME;
			}
			else {
				angle += 0.05f * speed * Enderman_STEP_TIME;
			}
			
			while (angle > 2 * PI) {
				angle -= 2 * PI;
			}
			while (angle < 0) {
				angle += 2 * PI;
			}
		}
	public:
		Enderman(MD2Model* model1,
			Terrain* terrain1,
			float terrainScale1) {
			model = model1;
			terrain = terrain1;
			terrainScale = terrainScale1;
			
			animTime = 0;
			timeUntilNextStep = 0;
			
			//Initialize certain fields to random values
			radius0 = 0.4f * randomFloat() + 0.25f;
			x0 = randomFloat() *
				(terrainScale * (terrain->width() - 1) - radius0) + radius0;
			z0 = randomFloat() *
				(terrainScale * (terrain->length() - 1) - radius0) + radius0;
			speed = 1.5f * randomFloat() + 2.0f;
			isTurningLeft = randomFloat() < 0.5f;
			angle = 2 * PI * randomFloat();
			timeUntilSwitchDir = randomFloat() * (20 * randomFloat() + 15);
		}
		
		//Advances the state of the Enderman by the specified amount of time, by
		//calling step() the appropriate number of times and adjusting animTime
		void advance(float dt) {
			//Adjust animTime
			animTime += 0.45f * dt * speed / radius0;
			if (animTime > -100000000 && animTime < 1000000000) {
				animTime -= (int)animTime;
				if (animTime < 0) {
					animTime += 1;
				}
			}
			else {
				animTime = 0;
			}
			
			//Call step() the appropriate number of times
			while (dt > 0) {
				if (timeUntilNextStep < dt) {
					dt -= timeUntilNextStep;
					step();
					timeUntilNextStep = Enderman_STEP_TIME;
				}
				else {
					timeUntilNextStep -= dt;
					dt = 0;
				}
			}
		}
		
		void draw() {
			if (model == NULL) {
				return;
			}
			
			float scale = radius0 / 1.5f;
			
			glPushMatrix();
			glTranslatef(x0, scale * 10.0f + y(), z0);
			glRotatef(90 - angle * 180 / PI, 0, 1, 0);
			glColor3f(1, 1, 1);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glScalef(scale, scale, scale);
			model->draw(animTime);
			glPopMatrix();
		}
		
		float x() {
			return x0;
		}
		
		float z() {
			return z0;
		}
		
		//Returns the current height of the Enderman on the terrain
		float y() {
			return terrainScale *heightAt(terrain, x0 / terrainScale, z0 / terrainScale);
		}
		
		float velocityX() {
			return speed * cos(angle);
		}
		
		float velocityZ() {
			return speed * sin(angle);
		}
		
		//Returns the approximate radius of the Enderman
		float radius() {
			return radius0;
		}
		
		//Returns the angle at which the Enderman is currently walking, in radians.
		//An angle of 0 indicates the positive x direction, while an angle of
		//PI / 2 indicates the positive z direction.
		float walkAngle() {
			return angle;
		}	
		
		//Adjusts the angle at which this Enderman is walking in response to a
		//collision with the specified Enderman
		void bounceOff(Enderman* otherEnderman) {
			float vx = velocityX();
			float vz = velocityZ();
			
			float dx = otherEnderman->x0 - x0;
			float dz = otherEnderman->z0 - z0;
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

struct EndermanPair {
	Enderman* Enderman1;
	Enderman* Enderman2;
};

const int MAX_QUADTREE_DEPTH = 6;
const int MIN_EndermanS_PER_QUADTREE = 2;
const int MAX_EndermanS_PER_QUADTREE = 5;

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
		//The Endermans in this, if this doesn't have any children
		set<Enderman*> Endermans;
		//The depth of this in the tree
		int depth;
		//The number of Endermans in this, including those stored in its children
		int numEndermans;
		
		//Adds a Enderman to or removes one from the children of this
		void fileEnderman(Enderman* Enderman, float x, float z, bool addEnderman) {
			//Figure out in which child(ren) the Enderman belongs
			for(int xi = 0; xi < 2; xi++) {
				if (xi == 0) {
					if (x - Enderman->radius() > centerX) {
						continue;
					}
				}
				else if (x + Enderman->radius() < centerX) {
					continue;
				}
				
				for(int zi = 0; zi < 2; zi++) {
					if (zi == 0) {
						if (z - Enderman->radius() > centerZ) {
							continue;
						}
					}
					else if (z + Enderman->radius() < centerZ) {
						continue;
					}
					
					//Add or remove the Enderman
					if (addEnderman) {
						children[xi][zi]->add(Enderman);
					}
					else {
						children[xi][zi]->remove(Enderman, x, z);
					}
				}
			}
		}
		
		//Creates children of this, and moves the Endermans in this to the children
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
			
			//Remove all Endermans from "Endermans" and add them to the new children
			for(set<Enderman*>::iterator it = Endermans.begin(); it != Endermans.end();
					it++) {
				Enderman* Enderman = *it;
				fileEnderman(Enderman, Enderman->x(), Enderman->z(), true);
			}
			Endermans.clear();
			
			hasChildren = true;
		}
		
		//Adds all Endermans in this or one of its descendants to the specified set
		void collectEndermans(set<Enderman*> &gs) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int z = 0; z < 2; z++) {
						children[x][z]->collectEndermans(gs);
					}
				}
			}
			else {
				for(set<Enderman*>::iterator it = Endermans.begin(); it != Endermans.end();
						it++) {
					Enderman* Enderman = *it;
					gs.insert(Enderman);
				}
			}
		}
		
		//Destroys the children of this, and moves all Endermans in its descendants
		//to the "Endermans" set
		void destroyChildren() {
			//Move all Endermans in descendants of this to the "Endermans" set
			collectEndermans(Endermans);
			
			for(int x = 0; x < 2; x++) {
				for(int z = 0; z < 2; z++) {
					delete children[x][z];
				}
			}
			
			hasChildren = false;
		}
		
		//Removes the specified Enderman at the indicated position
		void remove(Enderman* Enderman, float x, float z) {
			numEndermans--;
			
			if (hasChildren && numEndermans < MIN_EndermanS_PER_QUADTREE) {
				destroyChildren();
			}
			
			if (hasChildren) {
				fileEnderman(Enderman, x, z, false);
			}
			else {
				Endermans.erase(Enderman);
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
			numEndermans = 0;
			hasChildren = false;
		}
		
		~Quadtree() {
			if (hasChildren) {
				destroyChildren();
			}
		}
		
		//Adds a Enderman to this
		void add(Enderman* Enderman) {
			numEndermans++;
			if (!hasChildren && depth < MAX_QUADTREE_DEPTH &&
				numEndermans > MAX_EndermanS_PER_QUADTREE) {
				haveChildren();
			}
			
			if (hasChildren) {
				fileEnderman(Enderman, Enderman->x(), Enderman->z(), true);
			}
			else {
				Endermans.insert(Enderman);
			}
		}
		
		//Removes a Enderman from this
		void remove(Enderman* Enderman) {
			remove(Enderman, Enderman->x(), Enderman->z());
		}
		
		//Changes the position of a Enderman in this from the specified position to
		//its current position
		void EndermanMoved(Enderman* Enderman, float x, float z) {
			remove(Enderman, x, z);
			add(Enderman);
		}
		
		//Adds potential collisions to the specified set
		void potentialCollisions(vector<EndermanPair> &collisions) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int z = 0; z < 2; z++) {
						children[x][z]->potentialCollisions(collisions);
					}
				}
			}
			else {
				//Add all pairs (Enderman1, Enderman2) from Endermans
				for(set<Enderman*>::iterator it = Endermans.begin(); it != Endermans.end();
						it++) {
					Enderman* Enderman1 = *it;
					for(set<Enderman*>::iterator it2 = Endermans.begin();
							it2 != Endermans.end(); it2++) {
						Enderman* Enderman2 = *it2;
						//This test makes sure that we only add each pair once
						if (Enderman1 < Enderman2) {
							EndermanPair gp;
							gp.Enderman1 = Enderman1;
							gp.Enderman2 = Enderman2;
							collisions.push_back(gp);
						}
					}
				}
			}
		}
};

void potentialCollisions(vector<EndermanPair> &cs, Quadtree* quadtree) {
	quadtree->potentialCollisions(cs);
}

//Returns whether Enderman1 and Enderman2 are currently colliding
bool testCollision(Enderman* Enderman1, Enderman* Enderman2) {
	float dx = Enderman1->x() - Enderman2->x();
	float dz = Enderman1->z() - Enderman2->z();
	float r = Enderman1->radius() + Enderman2->radius();
	if (dx * dx + dz * dz < r * r) {
		float vx = Enderman1->velocityX() - Enderman2->velocityX();
		float vz = Enderman1->velocityZ() - Enderman2->velocityZ();
		return vx * dx + vz * dz < 0;
	}
	else {
		return false;
	}
}

void handleCollisions(vector<Enderman*> &Endermans, Quadtree* quadtree, int &numCollisions) {
	vector<EndermanPair> gps;
	potentialCollisions(gps, quadtree);
	for(unsigned int i = 0; i < gps.size(); i++) {
		EndermanPair gp = gps[i];
		
		Enderman* g1 = gp.Enderman1;
		Enderman* g2 = gp.Enderman2;
		if (testCollision(g1, g2)) {
			g1->bounceOff(g2);
			g2->bounceOff(g1);
			numCollisions++;
		}
	}
}

//Moves the Endermans over the given interval of time, without handling collisions
void moveEndermans(vector<Enderman*> &Endermans, Quadtree* quadtree, float dt) {
	for(unsigned int i = 0; i < Endermans.size(); i++) {
		Enderman* Enderman = Endermans[i];
		float oldX = Enderman->x();
		float oldZ = Enderman->z();
		Enderman->advance(dt);
		quadtree->EndermanMoved(Enderman, oldX, oldZ);
	}
}

//Advances the state of the Endermans over the indicated interval of time
void advance(vector<Enderman*> &Endermans, Quadtree* quadtree, float t, float &timeUntilHandleCollisions, int &numCollisions) {
	while (t > 0) {
		if (timeUntilHandleCollisions <= t) {
			moveEndermans(Endermans, quadtree, timeUntilHandleCollisions);
			handleCollisions(Endermans, quadtree, numCollisions);
			t -= timeUntilHandleCollisions;
			timeUntilHandleCollisions = TIME_BETWEEN_HANDLE_COLLISIONS;
		}
		else {
			moveEndermans(Endermans, quadtree, t);
			timeUntilHandleCollisions -= t;
			t = 0;
		}
	}
}

//Returns a vector of numEndermans new Endermans
vector<Enderman*> makeEndermans(int numEndermans, MD2Model* model, Terrain* terrain) {
	vector<Enderman*> Endermans;
	for(int i = 0; i < numEndermans; i++) {
		Endermans.push_back(new Enderman(model,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1)));
	}
	return Endermans;
}

vector<Enderman*> _Endermans;
Quadtree* _quadtree;

void update(int value) {
	_angle += 0.3f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	advance(_Endermans,
			_quadtree,
			0.025f,
			_timeUntilHandleCollisions,
			_numCollisions);
	
	glutPostRedisplay();
	glutTimerFunc(1000/60, update, 0);
}

#endif

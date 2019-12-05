
//creadits : www.videotutorialsrock.com, www.lighthouse3d.com

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
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_timer.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imageloader.h"
#include "md2model.h"
#include "vec3f.h"
#include "text3d.h"

#define ENTER 13
#define ESC 27
#define SPACEBAR 32

using namespace std;

//===========================================================================================================================

char title[] = "Proyek Akhir - FPS";
int res_x = 1366;
int res_y = 768;
float fov = 70.0f;
int is_fullscreen = 0;
const int refreshRate = 1000/60;
// controls
int keystates[256];
const char fwd = 'w';
const char fwd_caps = 'W';
const char back = 's';
const char back_caps = 'S';
const char lft = 'a';
const char lft_caps = 'A';
const char rgt = 'd';
const char rgt_caps = 'D';
const char crouch = 'c';
const char crouch_caps = 'C';
const int jump = SPACEBAR;

int sprint = 0;
int spectator = 0;
int pause = 0;
int mouseSensitivity = 3;
// angle of rotation for the camera direction
const int xOrigin = 683;
const int yOrigin = 384;
float angle_x = 0.0f;
float angle_y = 0.0f;
float deltaAngle_x = 0.0f;
float deltaAngle_y = 0.0f;
float speed_walk = 0.4f;
float speed_walk_temp = 0.2f;
float gravity = 1;
// current position of the camera
float height_player = 3;
float x=250.0f, y=10.0f, z=500.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=0.0f;

GLuint textureData[2];
float terrain_angle = 60.0f;
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
Terrain* terrainData[2];

const float PI = 3.1415926535f;
const int NUM_GUYS = 300;
//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 2000;
//The amount of time between each time that we handle collisions
const float TIME_BETWEEN_HANDLE_COLLISIONS = 0.01f;
//The amount by which the Guy class's step function advances the state of a guy
const float GUY_STEP_TIME = 0.01f;

//===========================================================================================================================

void GL_init();
void screenResize(int w, int h);
void toggle_fullscreen();
GLuint LoadTexture(char* filename, int generate);
void render3D();
void update(int value);
void init_lighting();
Terrain* loadTerrain(const char* filename, float height);
void cleanup();
void drawTerrain(Terrain* terrain);
void drawGround();
float heightAt(Terrain* terrain, float x, float z);
void key_press(unsigned char key, int xx, int yy);
void key_release(unsigned char key, int x, int y);
void specKey_press(int key, int xx, int yy);
void specKey_release(int key, int x, int y);
void key_calc(float terrainScale);
void camera(int x, int y);
void mouseButton(int button, int state, int x, int y);

//Returns a random float from 0 to < 1
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

//Represents a guy
class Guy {
	private:
		MD2Model* model;
		Terrain* terrain;
		float terrainScale; //The scaling factor for the terrain
		float x0;
		float z0;
		float animTime; //The current position in the animation of the model
		float radius0; //The approximate radius of the guy
		float speed;
		//The angle at which the guy is currently walking, in radians.  An angle
		//of 0 indicates the positive x direction, while an angle of PI / 2
		//indicates the positive z direction.  The angle always lies between 0
		//and 2 * PI.
		float angle;
		//The amount of time until step() should next be called
		float timeUntilNextStep;
		bool isTurningLeft; //Whether the guy is currently turning left
		float timeUntilSwitchDir; //The amount of time until switching direction
		
		//Advances the state of the guy by GUY_STEP_TIME seconds (without
		//altering animTime)
		void step() {
			//Update the turning direction information
			timeUntilSwitchDir -= GUY_STEP_TIME;
			while (timeUntilSwitchDir <= 0) {
				timeUntilSwitchDir += 20 * randomFloat() + 15;
				isTurningLeft = !isTurningLeft;
			}
			
			//Update the position and angle
			float maxX = terrainScale * (terrain->width() - 1) - radius0;
			float maxZ = terrainScale * (terrain->length() - 1) - radius0;
			
			x0 += velocityX() * GUY_STEP_TIME;
			z0 += velocityZ() * GUY_STEP_TIME;
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
					angle -= 0.5f * speed * GUY_STEP_TIME;
				}
				else {
					angle += 0.5f * speed * GUY_STEP_TIME;
				}
			}
			else if (isTurningLeft) {
				angle -= 0.05f * speed * GUY_STEP_TIME;
			}
			else {
				angle += 0.05f * speed * GUY_STEP_TIME;
			}
			
			while (angle > 2 * PI) {
				angle -= 2 * PI;
			}
			while (angle < 0) {
				angle += 2 * PI;
			}
		}
	public:
		Guy(MD2Model* model1,
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
		
		//Advances the state of the guy by the specified amount of time, by
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
					timeUntilNextStep = GUY_STEP_TIME;
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
			
			float scale = radius0 / 2.5f;
			
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
		
		//Returns the current height of the guy on the terrain
		float y() {
			return terrainScale *
				heightAt(terrain, x0 / terrainScale, z0 / terrainScale);
		}
		
		float velocityX() {
			return speed * cos(angle);
		}
		
		float velocityZ() {
			return speed * sin(angle);
		}
		
		//Returns the approximate radius of the guy
		float radius() {
			return radius0;
		}
		
		//Returns the angle at which the guy is currently walking, in radians.
		//An angle of 0 indicates the positive x direction, while an angle of
		//PI / 2 indicates the positive z direction.
		float walkAngle() {
			return angle;
		}	
		
		//Adjusts the angle at which this guy is walking in response to a
		//collision with the specified guy
		void bounceOff(Guy* otherGuy) {
			float vx = velocityX();
			float vz = velocityZ();
			
			float dx = otherGuy->x0 - x0;
			float dz = otherGuy->z0 - z0;
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

struct GuyPair {
	Guy* guy1;
	Guy* guy2;
};

const int MAX_QUADTREE_DEPTH = 6;
const int MIN_GUYS_PER_QUADTREE = 2;
const int MAX_GUYS_PER_QUADTREE = 5;

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
		//The guys in this, if this doesn't have any children
		set<Guy*> guys;
		//The depth of this in the tree
		int depth;
		//The number of guys in this, including those stored in its children
		int numGuys;
		
		//Adds a guy to or removes one from the children of this
		void fileGuy(Guy* guy, float x, float z, bool addGuy) {
			//Figure out in which child(ren) the guy belongs
			for(int xi = 0; xi < 2; xi++) {
				if (xi == 0) {
					if (x - guy->radius() > centerX) {
						continue;
					}
				}
				else if (x + guy->radius() < centerX) {
					continue;
				}
				
				for(int zi = 0; zi < 2; zi++) {
					if (zi == 0) {
						if (z - guy->radius() > centerZ) {
							continue;
						}
					}
					else if (z + guy->radius() < centerZ) {
						continue;
					}
					
					//Add or remove the guy
					if (addGuy) {
						children[xi][zi]->add(guy);
					}
					else {
						children[xi][zi]->remove(guy, x, z);
					}
				}
			}
		}
		
		//Creates children of this, and moves the guys in this to the children
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
			
			//Remove all guys from "guys" and add them to the new children
			for(set<Guy*>::iterator it = guys.begin(); it != guys.end();
					it++) {
				Guy* guy = *it;
				fileGuy(guy, guy->x(), guy->z(), true);
			}
			guys.clear();
			
			hasChildren = true;
		}
		
		//Adds all guys in this or one of its descendants to the specified set
		void collectGuys(set<Guy*> &gs) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int z = 0; z < 2; z++) {
						children[x][z]->collectGuys(gs);
					}
				}
			}
			else {
				for(set<Guy*>::iterator it = guys.begin(); it != guys.end();
						it++) {
					Guy* guy = *it;
					gs.insert(guy);
				}
			}
		}
		
		//Destroys the children of this, and moves all guys in its descendants
		//to the "guys" set
		void destroyChildren() {
			//Move all guys in descendants of this to the "guys" set
			collectGuys(guys);
			
			for(int x = 0; x < 2; x++) {
				for(int z = 0; z < 2; z++) {
					delete children[x][z];
				}
			}
			
			hasChildren = false;
		}
		
		//Removes the specified guy at the indicated position
		void remove(Guy* guy, float x, float z) {
			numGuys--;
			
			if (hasChildren && numGuys < MIN_GUYS_PER_QUADTREE) {
				destroyChildren();
			}
			
			if (hasChildren) {
				fileGuy(guy, x, z, false);
			}
			else {
				guys.erase(guy);
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
			numGuys = 0;
			hasChildren = false;
		}
		
		~Quadtree() {
			if (hasChildren) {
				destroyChildren();
			}
		}
		
		//Adds a guy to this
		void add(Guy* guy) {
			numGuys++;
			if (!hasChildren && depth < MAX_QUADTREE_DEPTH &&
				numGuys > MAX_GUYS_PER_QUADTREE) {
				haveChildren();
			}
			
			if (hasChildren) {
				fileGuy(guy, guy->x(), guy->z(), true);
			}
			else {
				guys.insert(guy);
			}
		}
		
		//Removes a guy from this
		void remove(Guy* guy) {
			remove(guy, guy->x(), guy->z());
		}
		
		//Changes the position of a guy in this from the specified position to
		//its current position
		void guyMoved(Guy* guy, float x, float z) {
			remove(guy, x, z);
			add(guy);
		}
		
		//Adds potential collisions to the specified set
		void potentialCollisions(vector<GuyPair> &collisions) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int z = 0; z < 2; z++) {
						children[x][z]->potentialCollisions(collisions);
					}
				}
			}
			else {
				//Add all pairs (guy1, guy2) from guys
				for(set<Guy*>::iterator it = guys.begin(); it != guys.end();
						it++) {
					Guy* guy1 = *it;
					for(set<Guy*>::iterator it2 = guys.begin();
							it2 != guys.end(); it2++) {
						Guy* guy2 = *it2;
						//This test makes sure that we only add each pair once
						if (guy1 < guy2) {
							GuyPair gp;
							gp.guy1 = guy1;
							gp.guy2 = guy2;
							collisions.push_back(gp);
						}
					}
				}
			}
		}
};

void potentialCollisions(vector<GuyPair> &cs, Quadtree* quadtree) {
	quadtree->potentialCollisions(cs);
}

//Returns whether guy1 and guy2 are currently colliding
bool testCollision(Guy* guy1, Guy* guy2) {
	float dx = guy1->x() - guy2->x();
	float dz = guy1->z() - guy2->z();
	float r = guy1->radius() + guy2->radius();
	if (dx * dx + dz * dz < r * r) {
		float vx = guy1->velocityX() - guy2->velocityX();
		float vz = guy1->velocityZ() - guy2->velocityZ();
		return vx * dx + vz * dz < 0;
	}
	else {
		return false;
	}
}

void handleCollisions(vector<Guy*> &guys,
					  Quadtree* quadtree,
					  int &numCollisions) {
	vector<GuyPair> gps;
	potentialCollisions(gps, quadtree);
	for(unsigned int i = 0; i < gps.size(); i++) {
		GuyPair gp = gps[i];
		
		Guy* g1 = gp.guy1;
		Guy* g2 = gp.guy2;
		if (testCollision(g1, g2)) {
			g1->bounceOff(g2);
			g2->bounceOff(g1);
			numCollisions++;
		}
	}
}

//Moves the guys over the given interval of time, without handling collisions
void moveGuys(vector<Guy*> &guys, Quadtree* quadtree, float dt) {
	for(unsigned int i = 0; i < guys.size(); i++) {
		Guy* guy = guys[i];
		float oldX = guy->x();
		float oldZ = guy->z();
		guy->advance(dt);
		quadtree->guyMoved(guy, oldX, oldZ);
	}
}

//Advances the state of the guys over the indicated interval of time
void advance(vector<Guy*> &guys,
			 Quadtree* quadtree,
			 float t,
			 float &timeUntilHandleCollisions,
			 int &numCollisions) {
	while (t > 0) {
		if (timeUntilHandleCollisions <= t) {
			moveGuys(guys, quadtree, timeUntilHandleCollisions);
			handleCollisions(guys, quadtree, numCollisions);
			t -= timeUntilHandleCollisions;
			timeUntilHandleCollisions = TIME_BETWEEN_HANDLE_COLLISIONS;
		}
		else {
			moveGuys(guys, quadtree, t);
			timeUntilHandleCollisions -= t;
			t = 0;
		}
	}
}

//Returns a vector of numGuys new guys
vector<Guy*> makeGuys(int numGuys, MD2Model* model, Terrain* terrain) {
	vector<Guy*> guys;
	for(int i = 0; i < numGuys; i++) {
		guys.push_back(new Guy(model,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1)));
	}
	return guys;
}

MD2Model* _model;
vector<Guy*> _guys;
float _angle = 0;
Quadtree* _quadtree;
//The amount of time until we next check for and handle all collisions
float _timeUntilHandleCollisions = 0;
int _numCollisions; //The total number of collisions that have occurred

//===========================================================================================================================

int main(int argc, char **argv) {
	// init GLUT and create window
	printf("initializing");
	glutInit(&argc, argv);
	GL_init();
		
	// register callbacks
	glutDisplayFunc(render3D);
	glutReshapeFunc(screenResize);
	glutIdleFunc(render3D);

	//keyboard input
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(key_press);
	glutKeyboardUpFunc(key_release);
	glutSpecialFunc(specKey_press);
	glutSpecialUpFunc(specKey_release);
	glutTimerFunc(25, update, 0);

	// camera & mouse input
	glutPassiveMotionFunc(camera);
	glutMouseFunc(mouseButton);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

//===========================================================================================================================

void GL_init(){
	glutInitWindowPosition(100,100);
	glutInitWindowSize(res_x,res_y);
	glutCreateWindow(title);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//glClearColor(0.498f, 0.83137f, 1.0f, 1.0f);
	glClearColor(0.1255f, 0.01961f, 0.1294f, 1.0f);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);
	//glEnable(GL_LIGHT3);
	//glEnable(GL_LIGHT4);
	//glEnable(GL_LIGHT5);
	//glEnable(GL_LIGHT6);
	//glEnable(GL_LIGHT7);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	
	//textureData[0] = LoadTexture("resources/textures/grass2.jpg", 1);
 	textureData[1] = LoadTexture("resources/textures/sand.jpg", 1);
 	terrainData[0] = loadTerrain("resources/textures/heightmap6.png", 20);
 	t3dInit();		 //Initialize text drawing functionality
	_model = MD2Model::load("blockybalboa.md2");	//Load the model
	if (_model != NULL) {
		_model->setAnimation("run");
	}
	_guys = makeGuys(NUM_GUYS, _model, terrainData[0]); //Create the guys
	//Compute the scaling factor for the terrain
	float scaledTerrainLength =
		TERRAIN_WIDTH / (terrainData[0]->width() - 1) * (terrainData[0]->length() - 1);
	//Construct and initialize the quadtree
	_quadtree = new Quadtree(0, 0, TERRAIN_WIDTH, scaledTerrainLength, 1);
	for(unsigned int i = 0; i < _guys.size(); i++) {
		_quadtree->add(_guys[i]);
	}
}

void screenResize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(fov, ratio, 0.1f, 3000.0f);
	glMatrixMode(GL_MODELVIEW);
}

void toggle_fullscreen() {
    if(!is_fullscreen){
        glutFullScreen();
        is_fullscreen = 1;
    } else if(is_fullscreen){
        glutReshapeWindow(res_x, res_y);
		glutPositionWindow(100,100);
        is_fullscreen = 0;
    }
}

GLuint LoadTexture(char* filename, int generate){
	int req_channels = 3; // 3 color channels of BMP-file   
	int width = 0, height = 0, channels = 0;
	unsigned char *data = stbi_load(filename, &width, &height, &channels, req_channels);	
	unsigned int ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	if(generate){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	}else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	stbi_image_free(data);
	return ID;
}

void render3D() {
	float scale = TERRAIN_WIDTH / (terrainData[0]->width() - 1);
	key_calc(scale);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	init_lighting();
	// Set the camera
	gluLookAt(	x, y, z,
			x+lx, y+ly, z+lz,
			0.0f, 1.0f, 0.0f);
	//The scaling factor for the terrain
	
	//glRotatef(150,0.0f, 1.0f, 0.0f);
	//glTranslatef(-280.0f, 0, -280.0f);
	//Draw the guys
	for(unsigned int i = 0; i < _guys.size(); i++) {
		_guys[i]->draw();
	}
	
	//Draw the terrain
	glScalef(scale, scale, scale);
	drawTerrain(terrainData[0]);
	
    glutSwapBuffers();
    glutPostRedisplay();
}

void update(int value) {
	_angle += 0.3f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	advance(_guys,
			_quadtree,
			0.025f,
			_timeUntilHandleCollisions,
			_numCollisions);
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

void init_lighting(){
	//ambient light
	static float ambientColor[8][4] ={0.4, 0.4, 0.4, 1,
									  0.2, 0.2, 0.2, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor[0]);
	
	//positioned light
	static float lightColor[8][4] ={0.5, 0.5, 0.5, 1,
								    0.5, 0.2, 0.2, 1};
	static float lightPos[8][4] ={0, 10000, 0, 1,
								  -1.0, 0.5, 0.5, 1};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos[0]);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor[1]);
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPos[1]);
	
	static float lightAmb[8][4] ={0, 0, 0, 0,
								  0, 0, 0, 0};
	static float lightSpec[8][4] ={1, 1, 1, 1,
								   1, 1, 1, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec[0]);
}

Terrain* loadTerrain(const char* filename, float height) {
	int req_channels = 3; // 3 color channels of BMP-file   
	int data_width = 0, data_height = 0, channels = 0;
	unsigned char *data = stbi_load(filename, &data_width, &data_height, &channels, req_channels);
	
	Terrain* t = new Terrain(data_width, data_height);
	for(int y = 0; y < data_height; y++) {
		for(int x = 0; x < data_width; x++) {
			unsigned char color = (unsigned char)data[3 * (y * data_width + x)];
			float h = height * ((color / 32.0f)-6.6f);
			t->setHeight(x, y, h);
		}
	}
	stbi_image_free(data);
	t->computeNormals();
	return t;
}

void cleanup() {
	delete terrainData;
	delete _model;
	
	for(unsigned int i = 0; i < _guys.size(); i++) {
		delete _guys[i];
	}
	t3dCleanup();
}

void drawTerrain(Terrain* terrain){	
	int scaling = 2;
	Vec3f normal;								
	glColor3f(0.9059f, 0.9412f, 0.6784f);
	//glBindTexture(GL_TEXTURE_2D, textureData[1]);
	//glEnable(GL_TEXTURE_2D);
	for(int z = 0; z < terrain->length()-scaling; z+=scaling) {
		glBegin(GL_TRIANGLE_STRIP);
		//int a, b;
		for(int x = 0; x < terrain->width(); x+=scaling) {
			
			//glTexCoord2f(a, b);
			normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			
			//glTexCoord2f(a, b);
			normal = terrain->getNormal(x, z + scaling);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + scaling), z + scaling);
		}
		glEnd();
		//glDisable(GL_TEXTURE_2D);
	}
}

void drawGround(){										
	//glColor3ub(150, 190, 150);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textureData[0]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glTexCoord2f(100.0, 0.0);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glTexCoord2f(100.0, 100.0);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glTexCoord2f(0.0, 100.0);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
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

void key_press(unsigned char key, int xx, int yy) {
	int mod = glutGetModifiers();
	if (mod == GLUT_ACTIVE_ALT){
		if (key == ENTER){
			toggle_fullscreen();
		}
	}
	
	keystates[key] = 1;
	
	if(keystates[ESC]){
		switch(pause){
			case 0:
				pause = 1;
				glutSetCursor(GLUT_CURSOR_INHERIT);
				break;
			case 1:
				pause = 0;
				glutSetCursor(GLUT_CURSOR_NONE);
				break;
		}
	}
}

void key_release(unsigned char key, int x, int y){
	keystates[key] = 0;
}

void specKey_press(int key, int xx, int yy) {

    switch (key) {
    	
    }
} 

void specKey_release(int key, int x, int y) { 	

    switch (key) {
    	
    }
} 

void key_calc(float terrainScale){
	float height_terrain = (terrainScale * heightAt(terrainData[0], x/terrainScale, z/terrainScale)) + height_player;
	if(!pause){
		if((keystates[fwd] || keystates[fwd_caps]) && !(keystates[back] || keystates[back_caps])){
			x += speed_walk * lx;
			z += speed_walk * lz;
			if(spectator == 1){
				y += speed_walk * ly;
			}
		}
		if(!(keystates[fwd] || keystates[fwd_caps]) && (keystates[back] || keystates[back_caps])){
			x -= speed_walk * lx;
			z -= speed_walk * lz;
			if(spectator == 1){
				y -= speed_walk * ly;
			}
		}
		if((keystates[lft] || keystates[lft_caps]) && !(keystates[rgt] || keystates[rgt_caps])){
			x += speed_walk * lz;
			z -= speed_walk * lx;
		}
		if(!(keystates[lft] || keystates[lft_caps]) && (keystates[rgt] || keystates[rgt_caps])){
			x -= speed_walk * lz;
			z += speed_walk * lx;
		}
		
		/*
		if(keystates[crouch] || keystates[crouch_caps]){
			y = height_terrain/2;
			speed_walk = speed_walk_temp;
		}else if(!keystates[crouch] || !keystates[crouch_caps]){
			if(y<height_terrain && spectator != 1){
				y = height_terrain;
				speed_walk = speed_walk_temp;
			}
		}
		*/
		
		if(keystates[jump]){
			if(y<height_terrain + 4.0f){
				y += gravity;
			}else if(y>= height_terrain + 4.0f){
				keystates[jump] = 0;
			}
		}
		if(!keystates[jump] && y>height_terrain && !spectator){
			y -= gravity;
		}
		if(y <= height_terrain && (!keystates[crouch] || !keystates[crouch_caps])){
			y = height_terrain;
		}
	}
}

void camera(int x, int y) {
	if(!pause){
		deltaAngle_x += (x - xOrigin) * 0.0002f * mouseSensitivity;
		deltaAngle_y += (y - yOrigin) * 0.0002f * mouseSensitivity;
		
		if(deltaAngle_y > 1.0f){
			deltaAngle_y = 1.0f;
		}else if(deltaAngle_y < -1.0f){
			deltaAngle_y = -1.0f;
		}
		
		lx = sin(angle_x + deltaAngle_x);
		ly = -sin(angle_y + deltaAngle_y);
		lz = -cos(angle_x + deltaAngle_x);
		
		glutWarpPointer(res_x/2, res_y/2);
	}
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON) {
		if (state != GLUT_UP) {
			
		}
		else if (state == GLUT_UP) {

		}
	}
}

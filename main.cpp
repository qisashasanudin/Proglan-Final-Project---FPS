
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
#include "terrain.h"
#include "enderman.h"

#define ENTER 13
#define ESC 27
#define SPACEBAR 32

using namespace std;

//===========================================================================================================================

char title[] = "Proyek Akhir - FPS";
int res_x = 1366;
int res_y = 768;
float fov = 80.0f;
int is_fullscreen = 0;
const int framerate = 1000/60;
float sky_r=0.1255f, sky_g=0.01961f, sky_b=0.1294f;		//blue sky : 0.498f, 0.83137f, 1.0f;
float view_dist = 2000.0f;

// controls
int keystates[256];
int modifierstates[256];
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
float speed_walk = 0.8f;
float speed_walk_temp = 0.2f;
float gravity = 1;
// current position of the camera
float height_player = 2.0f;
float x=300.0f, y=10.0f, z=600.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=0.0f;

GLuint textureData[2];
Terrain* terrainData[2];

//===========================================================================================================================

void GL_init();
void screenResize(int w, int h);
void toggle_fullscreen();
GLuint LoadTexture(char* filename, int generate);
void render3D();
void init_lighting();
Terrain* loadTerrain(const char* filename, float height);
void cleanup();
void drawTerrain(Terrain* terrain);
void drawGround();
void key_press(unsigned char key, int xx, int yy);
void key_release(unsigned char key, int x, int y);
void specKey_press(int key, int xx, int yy);
void specKey_release(int key, int x, int y);
void key_calc(float terrainScale);
void camera(int x, int y);
void mouseButton(int button, int state, int x, int y);

//===========================================================================================================================

int main(int argc, char **argv) {
	// init GLUT and create window
	printf("initializing\n");
	glutInit(&argc, argv);
	GL_init();
		
	// register callbacks
	glutDisplayFunc(render3D);
	glutReshapeFunc(screenResize);

	//keyboard input
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(key_press);
	glutKeyboardUpFunc(key_release);
	glutSpecialFunc(specKey_press);
	glutSpecialUpFunc(specKey_release);
	glutTimerFunc(framerate, update, 0);

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
	glClearColor(sky_r, sky_g, sky_b, 1.0f);
	glutSetCursor(GLUT_CURSOR_NONE);
	toggle_fullscreen();
	
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
	glEnable(GL_FOG);
	
	//textureData[0] = LoadTexture("resources/textures/grass2.jpg", 1);
 	textureData[1] = LoadTexture("resources/textures/sand.jpg", 1);
 	terrainData[0] = loadTerrain("resources/textures/heightmap6.png", 20);
 	t3dInit();		 //Initialize text drawing functionality
	_model = MD2Model::load("blockybalboa.md2");	//Load the model
	if (_model != NULL) {
		_model->setAnimation("run");
	}
	_Endermans = makeEndermans(NUM_EndermanS, _model, terrainData[0]); //Create the Endermans
	//Compute the scaling factor for the terrain
	float scaledTerrainLength =
		TERRAIN_WIDTH / (terrainData[0]->width() - 1) * (terrainData[0]->length() - 1);
	//Construct and initialize the quadtree
	_quadtree = new Quadtree(0, 0, TERRAIN_WIDTH, scaledTerrainLength, 1);
	for(unsigned int i = 0; i < _Endermans.size(); i++) {
		_quadtree->add(_Endermans[i]);
	}
}

void screenResize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(fov, ratio, 0.1f, view_dist);
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
	

	
	GLfloat fogColor[]={sky_r, sky_g, sky_b, 1};
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_MODE, GL_LINEAR); //GL_EXP, GL_EXP2
	glFogf(GL_FOG_START,view_dist/5);
	glFogf(GL_FOG_END,view_dist);
	//glFogf(GL_FOG_DENSITY, 0.05f);
	
	//Draw the Endermans
	for(unsigned int i = 0; i < _Endermans.size(); i++) {
		_Endermans[i]->draw();
	}
	
	//Draw the terrain
	glScalef(scale, scale, scale);
	drawTerrain(terrainData[0]);
	
    glutSwapBuffers();
    glutPostRedisplay();
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
	delete textureData;
	delete terrainData;
	delete _model;
	
	for(unsigned int i = 0; i < _Endermans.size(); i++) {
		delete _Endermans[i];
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

void key_press(unsigned char key, int xx, int yy) {	
	keystates[key] = 1;
	modifierstates[glutGetModifiers()] = 1;
	
	if(glutGetModifiers() & GLUT_ACTIVE_ALT){
		if (key == ENTER){
			toggle_fullscreen();
		}
	}
	if(keystates[ESC]){
		cleanup();
	}
}

void key_release(unsigned char key, int x, int y){
	keystates[key] = 0;
	memset(modifierstates, 0, sizeof(modifierstates));
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
		if(!keystates[jump] && !spectator){
			y -= gravity;
		}
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
		
		/*
		
			if (keystates['w'] || keystates['W']){
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
			speed_walk = speed_walk_temp*3;
		}else{
			speed_walk = speed_walk_temp;
		}
	}
		*/
		
		
		if((keystates[lft] || keystates[lft_caps]) && !(keystates[rgt] || keystates[rgt_caps])){
			x += speed_walk * lz;
			z -= speed_walk * lx;
		}
		if(!(keystates[lft] || keystates[lft_caps]) && (keystates[rgt] || keystates[rgt_caps])){
			x -= speed_walk * lz;
			z += speed_walk * lx;
		}
		
		if(keystates[jump]){
			if(y < height_terrain + height_player*2){
				y += gravity/1.5;
			}else if(y >= height_terrain + height_player*2){
				keystates[jump] = 0;
			}
		}
		}if(y <= height_terrain && (!keystates[crouch] && !keystates[crouch_caps]) && !spectator){
			y = height_terrain;
			speed_walk = speed_walk_temp;
		}else if(y <= (height_terrain - height_player/2) && (keystates[crouch] || keystates[crouch_caps]) && !spectator){
			y = (height_terrain - height_player/2);
			speed_walk = speed_walk_temp/2;
	}
}

void camera(int x, int y) {
	if(!pause){
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer(res_x/2, res_y/2);
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
	}else if(pause){
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state != GLUT_UP) {
			
		}
		else if (state == GLUT_UP) {

		}
	}
}

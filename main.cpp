
//Referensi : www.videotutorialsrock.com, www.lighthouse3d.com

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

//we didn't use SDL
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
#include "linkedlist.h"
//#include "bullet.h"

#define ENTER 13
#define ESC 27

using namespace std;

//===========================================================================================================================

char title[] = "Proyek Akhir - FPS";
int res_x = 1366;
int res_y = 768;
float fov = 80.0f;
int is_fullscreen = 0;
const int framerate = 1000/60;
float sky_r=0.5843f, sky_g=0.7922f, sky_b=1.0f;		//end world : sky_r=0.1255f, sky_g=0.01961f, sky_b=0.1294f;
float view_dist = 10000.0f;
int map=1, mode=1;
int terrainQuality = 1;
int NUM_EndermanS = 500;
// camera's initial position
float x=4027.06f, y=0.0f, z=1313.62f;
float lx=0.0f, ly=0.0f, lz=0.0f;

// controls
int keystates[256];
struct Node* letter = NULL;

int spectator = 0;
int pause = 0;
int mouseSensitivity = 3;
// angle of rotation for the camera direction
float xOrigin = 683.0f;
float yOrigin = 384.0f;
float angle_x = 0.0f;
float angle_y = 0.0f;
float deltaAngle_x = 0.0f;
float deltaAngle_y = 0.0f;
float speed_walk = 0.3f;
float speed_walk_temp = 0.3f;
float height_player = 3.0f;
float height_player_temp = 3.0f;
float gravity = 0.98f;
float speed = 0;
float time_falling = 0.03;

GLuint textureData[2];
Terrain* terrainData[2];
MD2Model* _model;
GLuint displayListId;

//===========================================================================================================================

void linkedlist_init();
void GL_init();
void screenResize(int w, int h);
void toggle_fullscreen();
GLuint LoadTexture(char* filename, int generate);
void renderScene();
void init_lighting();
void init_fog();
void cleanup();
void drawTerrain(Terrain* terrain);
void key_press(unsigned char key, int xx, int yy);
void key_release(unsigned char key, int x, int y);
void control(float terrainScale);
void camera(int x, int y);
void mouseButton(int button, int state, int x, int y);

//===========================================================================================================================

int main(int argc, char **argv) {
	linkedlist_init();
	// init GLUT and create window	
	glutInit(&argc, argv);
	GL_init();
		
	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(screenResize);

	//keyboard input
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(key_press);
	glutKeyboardUpFunc(key_release);
	glutTimerFunc(framerate, update, 0);

	// camera & mouse input
	glutPassiveMotionFunc(camera);
	glutMouseFunc(mouseButton);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

//===========================================================================================================================

void linkedlist_init(){
	/*
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
	const char sprint = 'e';
	const char sprint_caps = 'E';
	const int jump = ' ';
	*/
	insert(&letter, 'w');
	insert(&letter, 'W');
	insert(&letter, 's');
	insert(&letter, 'S');
	insert(&letter, 'a');
	insert(&letter, 'A');
	insert(&letter, 'd');
	insert(&letter, 'D');
	insert(&letter, 'c');
	insert(&letter, 'C');
	insert(&letter, 'e');
	insert(&letter, 'E');
	insert(&letter, ' ');
}

void GL_init(){
	glutInitWindowPosition(100,100);
	glutInitWindowSize(res_x,res_y);
	glutCreateWindow(title);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	switch(map){
		case 1:
			terrainData[0] = loadTerrain("resources/textures/sukabumi.png", 20);
			break;
		case 2:
			terrainData[0] = loadTerrain("resources/textures/hawaii.png", 20);
			break;
	}
	switch(mode){
		case 1:
			textureData[0] = LoadTexture("resources/textures/grass2.jpg", 1);
			sky_r=0.5843f;
			sky_g=0.7922f;
			sky_b=1.0f;
			break;
		case 2:
			textureData[0] = LoadTexture("resources/textures/endstone.png", 1);
			sky_r=0.1255f;
			sky_g=0.01961f;
			sky_b=0.1294f;
			break;
	}
	
	glClearColor(sky_r, sky_g, sky_b, 1.0f);
	glEnable(GL_DEPTH_TEST);
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
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_FOG);
	glEnable(GL_CULL_FACE);
 	
 	//t3dInit();		 //Initialize text drawing functionality
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
	
	displayListId = glGenLists(1);
	glNewList(displayListId, GL_COMPILE);
		drawTerrain(terrainData[0]);
		init_lighting();
		init_fog();
	glEndList();
}

void drawTerrain(Terrain* terrain){
	Vec3f normal;								
	//glColor3f(0.9059f, 0.9412f, 0.6784f);
	glBindTexture(GL_TEXTURE_2D, textureData[0]);
	glEnable(GL_TEXTURE_2D);
	for(int z = 0; z < terrain->length()-terrainQuality; z+=terrainQuality) {
		int a=0, b=0;
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < terrain->width(); x+=terrainQuality) {
			glTexCoord2f(a, b);
			normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			a = !a;
			glTexCoord2f(a, b);
			normal = terrain->getNormal(x, z + terrainQuality);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + terrainQuality), z + terrainQuality);
			b = !b;
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void init_lighting(){
	//ambient light
	static float ambientColor[8][4] ={0.4, 0.4, 0.4, 1,
									  0.2, 0.2, 0.2, 1};
	static float lightColor[8][4] ={0.5, 0.5, 0.5, 1,
								    0.5, 0.2, 0.2, 1};
	static float lightPos[8][4] ={0, 10000, 0, 1,
								  -1.0, 0.5, 0.5, 1};
	static float lightSpec[8][4] ={1, 1, 1, 1,
								   1, 1, 1, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor[0]);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos[0]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec[0]);
	//static float lightAmb[8][4] ={0.4, 0.4, 0.4, 1,
	//							  0.2, 0.2, 0.2, 1};
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor[1]);
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPos[1]);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb[0]);
}

void init_fog(){
	GLfloat fogColor[]={sky_r, sky_g, sky_b, 1};
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_MODE, GL_LINEAR); //GL_EXP, GL_EXP2
	glFogf(GL_FOG_START,1);
	glFogf(GL_FOG_END,view_dist);
	//glFogf(GL_FOG_DENSITY, 0.05f);
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

void renderScene() {
	float scale = TERRAIN_WIDTH / (terrainData[0]->width() - 1);
	control(scale);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set the camera
	gluLookAt(	x, y, z,
			x+lx, y+ly, z+lz,
			0.0f, 1.0f, 0.0f);
	/*
	glPushMatrix();
	glColor3f(0.5,0.5,0.5);
	glTranslatef(x, y-0.2f, z);
	glRotatef(180-(angle_x + deltaAngle_x)*180.0/3.14, 0.0, 1.0, 0.0);
	glRotatef((angle_y + deltaAngle_y)*120.0/3.14, 1.0, 0.0, 0.0);
	glutSolidCone(0.1f,1.0f,10,4);
	glPopMatrix();
	*/
	//Draw the Endermans
	glCullFace(GL_FRONT);
	for(unsigned int i = 0; i < _Endermans.size(); i++) {
		_Endermans[i]->draw();
	}
	glCullFace(GL_BACK);
	
	//Draw the terrain
	glScalef(scale, scale, scale);
	glCallList(displayListId);
	
    glutSwapBuffers();
    glutPostRedisplay();
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

void key_press(unsigned char key, int xx, int yy) {	
	keystates[key] = 1;

	if(glutGetModifiers() & GLUT_ACTIVE_ALT){
		if (key == ENTER){
			toggle_fullscreen();
		}
	}
	if(keystates['`'] || keystates['~']){
		spectator = !spectator;
	}
	if(keystates[ESC]){
		pause = !pause;
	}
	if(keystates['=']){
		cleanup();
		exit(0);
	}
}

void key_release(unsigned char key, int x, int y){
	keystates[key] = 0;
}

void control(float terrainScale){
	float height_terrain = (terrainScale * heightAt(terrainData[0], x/terrainScale, z/terrainScale)) + height_player;
	
	if(!pause){
		if((!keystates[GetNth(letter, 8)] && !keystates[GetNth(letter, 9)]) && (!keystates[GetNth(letter, 10)] && !keystates[GetNth(letter, 11)])){
			height_player = height_player_temp;
			speed_walk = speed_walk_temp;
		}else if(keystates[GetNth(letter, 10)] || keystates[GetNth(letter, 11)]){
			if(spectator){
				speed_walk = speed_walk_temp*50.0f;
			}else{
				speed_walk = speed_walk_temp*2.0f;
			}
		}else if(keystates[GetNth(letter, 8)] || keystates[GetNth(letter, 9)]){
			height_player = height_player_temp/2;
			speed_walk = speed_walk_temp/2;
		}
		if((keystates[GetNth(letter, 0)] || keystates[GetNth(letter, 1)]) && !(keystates[GetNth(letter, 2)] || keystates[GetNth(letter, 3)])){
			x += speed_walk * lx;
			z += speed_walk * lz;
			if(spectator){
				y += speed_walk * ly;
			}
		}
		if(!(keystates[GetNth(letter, 0)] || keystates[GetNth(letter, 1)]) && (keystates[GetNth(letter, 2)] || keystates[GetNth(letter, 3)])){
			x -= speed_walk * lx;
			z -= speed_walk * lz;
			if(spectator){
				y -= speed_walk * ly;
			}
		}		
		if((keystates[GetNth(letter, 4)] || keystates[GetNth(letter, 5)]) && !(keystates[GetNth(letter, 6)] || keystates[GetNth(letter, 7)])){
			x += speed_walk * lz;
			z -= speed_walk * lx;
		}
		if(!(keystates[GetNth(letter, 4)] || keystates[GetNth(letter, 5)]) && (keystates[GetNth(letter, 6)] || keystates[GetNth(letter, 7)])){
			x -= speed_walk * lz;
			z += speed_walk * lx;
		}
		if(keystates[GetNth(letter, 12)]){
			if(y <= height_terrain+0.2){
				speed = 0.4;
			}
		}
	}
	if(!spectator){
		y += speed;
		time_falling += 0.003;
		speed -= gravity * time_falling;
		if(y <= height_terrain){
			y = height_terrain;
			time_falling = 0.03;
			speed = 0;
		}
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

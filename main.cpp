#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
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

#define ENTER 13
#define ESC 27
#define SPACEBAR 32

using namespace std;

//===========================================================================================================================

char title[] = "Proyek Akhir - FPS";
int res_x = 1366;
int res_y = 768;
float fov = 60.0f;
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
int spectator = 1;
int pause = 0;
int mouseSensitivity = 3;
// angle of rotation for the camera direction
const int xOrigin = 683;
const int yOrigin = 384;
float angle_x = 0.0f;
float angle_y = 0.0f;
float deltaAngle_x = 0.0f;
float deltaAngle_y = 0.0f;
float speed_walk = 0.3f;
float speed_walk_temp = 0.15f;
float gravity = 0.3f;
// current position of the camera
float height_player = 1.8f;
float x=0.0f, y=1.8f, z=5.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;

GLuint textures[2];

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
Terrain* terrainData;

const float FLOOR_TEXTURE_SIZE = 15.0f; //The size of each floor "tile"
float _angle = 30.0f;
MD2Model* modelData;
int _textureId;
//The forward position of the guy relative to an arbitrary floor "tile"
float _guyPos = 0;

//===========================================================================================================================

void GL_init();
void screenResize(int w, int h);
void toggle_fullscreen();
GLuint LoadTexture(char* filename, int generate);
void render3D();
void init_lighting();
Terrain* loadTerrain(const char* filename, float height);
void cleanup();
void drawTerrain();
void drawFlatGround();
void drawSnowMan();
void update(int value);
void key_press(unsigned char key, int xx, int yy);
void key_release(unsigned char key, int x, int y);
void specKey_press(int key, int xx, int yy);
void specKey_release(int key, int x, int y);
void key_calc();
void camera(int x, int y);
void mouseButton(int button, int state, int x, int y);

//===========================================================================================================================

int main(int argc, char **argv) {
	// init GLUT and create window
	printf("initializing");
	glutInit(&argc, argv);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(res_x,res_y);
	glutCreateWindow(title);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glClearColor(0.498f, 0.83137f, 1.0f, 1.0f);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	// OpenGL init
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
	glutTimerFunc(5, update, 0);

	// camera & mouse input
	glutPassiveMotionFunc(camera);
	glutMouseFunc(mouseButton);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

//===========================================================================================================================

void GL_init(){
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
	
	textures[0] = LoadTexture("resources/textures/grass2.jpg", 1);
 	//textures[1] = LoadTexture("resources/textures/sand.jpg", 1);
 	terrainData = loadTerrain("resources/textures/heightmap6.png", 20);
 	modelData = MD2Model::load("tallguy.md2");
	if(modelData != NULL){
		modelData->setAnimation("run");
	}
}

void screenResize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(fov, ratio, 0.1f, 1000.0f);
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
	key_calc();
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
	init_lighting();
	// Set the camera
	gluLookAt(	x, y, z,
			x+lx, y+ly, z+lz,
			0.0f, 1.0f, 0.0f);
			
	//Draw the guy
	if (modelData != NULL) {
		glPushMatrix();
		glTranslatef(0, 2, 0);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.1f, 0.2f, 0.1f);
		modelData->draw();
		glPopMatrix();
		glTranslatef(0, -2, 0);
	}
			
	drawTerrain();
	drawFlatGround();
	
    glutSwapBuffers();
    glutPostRedisplay();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void init_lighting(){
	//ambient light
	static float ambientColor[8][4] ={0.6, 0.6, 0.6, 1,
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
	delete modelData;
}

void drawTerrain(){	
	int scaling = 2;
	Vec3f normal;								
	glColor3f(0.824, 0.733, 0.639);
	glRotatef(150,0.0f, 1.0f, 0.0f);
	glTranslatef(-280.0f, 0, -280.0f);
	for(int z = 0; z < terrainData->length()-scaling; z+=scaling) {
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < terrainData->width(); x+=scaling) {

			normal = terrainData->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrainData->getHeight(x, z), z);

			normal = terrainData->getNormal(x, z + scaling);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrainData->getHeight(x, z + scaling), z + scaling);
		}
		glEnd();
	}
}

void drawFlatGround(){										
	//glColor3ub(150, 190, 150);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(400.0f, 0, 400.0f);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glNormal3f(0, 1, 0);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glTexCoord2f(100.0, 0.0);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glTexCoord2f(100.0, 100.0);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glTexCoord2f(0.0, 100.0);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	for(int i=-3; i<3; i++){
			glPushMatrix();
			glTranslatef(i*10.0,0,10.0);
			drawSnowMan();
			glPopMatrix();
        }
}

void drawSnowMan() {
	glColor3f(1.0f, 1.0f, 1.0f);
// Draw Body
	glTranslatef(0.0f ,0.75f, 0.0f);
	glutSolidSphere(0.75f,20,20);
// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f,20,20);
// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f,10,10);
	glPopMatrix();
// Draw Nose
	glColor3f(1.0f, 0.5f , 0.5f);
	glRotatef(0.0f,1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f,0.5f,10,2);
}

void update(int value) {
	_angle += 0.7f;
	if (_angle > 360) {
		_angle -= 360;
	}
	//Advance the animation
	if (modelData != NULL) {
		modelData->advance(0.025f);
	}
	//Update _guyPos
	_guyPos += 0.08f;
	while (_guyPos > FLOOR_TEXTURE_SIZE) {
		_guyPos -= FLOOR_TEXTURE_SIZE;
	}
	glutPostRedisplay();
	glutTimerFunc(5, update, 0);
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

void key_calc(){
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
		if(keystates[crouch] || keystates[crouch_caps]){
			if(y>height_player/2){
				y -= gravity;
				speed_walk = speed_walk_temp;
			}
		}else if(!keystates[crouch] || !keystates[crouch_caps]){
			if(y<height_player && spectator != 1){
				y += gravity;
				speed_walk = speed_walk_temp*2;
			}
		}
		
		if(keystates[jump]){
			if(y<3.0f){
				y += gravity;
			}else if(y>=3.0f){
				keystates[jump] = 0;
			}
		}
		if(!keystates[jump] && y>height_player && !spectator){
			if(spectator != 1){
				y -= gravity;
			}
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

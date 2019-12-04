#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_timer.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENTER 13
#define ESC 27
#define SPACEBAR 32

//===========================================================================================================================

char title[] = "Proyek Akhir - FPS";
int res_x = 1366;
int res_y = 768;
float fov = 60.0f;
int is_fullscreen = 0;
const int refreshRate = 1000/60;
// controls
int keystates[256];
const char forward = 'w';
const char forward_caps = 'W';
const char back = 's';
const char back_caps = 'S';
const char left = 'a';
const char left_caps = 'A';
const char right = 'd';
const char right_caps = 'D';
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
float speed_walk = 0.03f;
float gravity = 0.03f;
// current position of the camera
float height_player = 1.8f;
float x=0.0f, y=1.8f, z=5.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;

GLuint textures[2];

//===========================================================================================================================

void GL_init();
void screenResize(int w, int h);
void toggle_fullscreen();
GLuint LoadTexture(char* filename, int generate);
void render3D();
void init_lighting();
void drawGround();
void drawGrid();
void drawSnowMan();
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
	
	textures[0] = LoadTexture("resources/grass.jpg", 1);
 	textures[1] = LoadTexture("resources/dirtblock.jpg", 1);
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
	
	drawGround();
// Draw 36 SnowMen
	int i,j;
	for(i=-3; i<3; i++){
		for(j=-3; j<3; j++){
			glPushMatrix();
			glTranslatef(i*10.0,0,j * 10.0);
			drawSnowMan();
			glPopMatrix();
        }
    }
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

void drawGround(){										
	//glColor3ub(150, 190, 150);
	glColor3f(1.0f, 1.0f, 1.0f);
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
}

void drawGrid(){																	
	float i;
	for(i = -500; i <= 500; i += 5)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);							
			glVertex3f(-500, 0, i);									
			glVertex3f(500, 0, i);

			glVertex3f(i, 0, -500);								
			glVertex3f(i, 0, 500);
		glEnd();
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
		if((keystates[forward] || keystates[forward_caps]) && !(keystates[back] || keystates[back_caps])){
			x += speed_walk * lx;
			z += speed_walk * lz;
			if(spectator == 1){
				y += speed_walk * ly;
			}
		}
		if(!(keystates[forward] || keystates[forward_caps]) && (keystates[back] || keystates[back_caps])){
			x -= speed_walk * lx;
			z -= speed_walk * lz;
			if(spectator == 1){
				y -= speed_walk * ly;
			}
		}
		if((keystates[left] || keystates[left_caps]) && !(keystates[right] || keystates[right_caps])){
			x += speed_walk * lz;
			z -= speed_walk * lx;
		}
		if(!(keystates[left] || keystates[left_caps]) && (keystates[right] || keystates[right_caps])){
			x -= speed_walk * lz;
			z += speed_walk * lx;
		}
		if(keystates[crouch] || keystates[crouch_caps]){
			if(y>height_player/2){
				y -= 0.01f;
				speed_walk = 0.03f/3;
			}
		}else if(!keystates[crouch] || !keystates[crouch_caps]){
			if(y<height_player && spectator != 1){
				y += 0.01f;
				speed_walk = 0.03f;
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

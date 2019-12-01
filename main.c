#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_timer.h>

#define ENTER 13
#define ESC 27
#define SPACEBAR 32

//===========================================================================================================================

char title[] = "Saya sedih sekali";
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
// angle of rotation for the camera direction
const int xOrigin = 683;
const int yOrigin = 384;
float angle_x = 0.0f;
float angle_y = 0.0f;
float deltaAngle_x = 0.0f;
float deltaAngle_y = 0.0f;
float speed_walk = 0.03f;
float gravity = 0.02f;
// current position of the camera
float height_player = 1.8f;
float x=0.0f, y=1.8f, z=5.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;


//===========================================================================================================================

void screenResize(int w, int h);
void toggle_fullscreen();
void render3D(void);
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
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(res_x,res_y);
	glutCreateWindow(title);
	glutSetCursor(GLUT_CURSOR_NONE);

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

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

//===========================================================================================================================

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

void render3D(void) {
	key_calc();
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
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
}

void drawGround(){																	
	glColor3ub(150, 190, 150);	
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();
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
			if(y>height_player){
				y -= 0.05f;
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
			y -= gravity;
		}
	}
}

void camera(int x, int y) {
	if(!pause){
		deltaAngle_x += (x - xOrigin) * 0.001f;
		deltaAngle_y += (y - yOrigin) * 0.001f;
		
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


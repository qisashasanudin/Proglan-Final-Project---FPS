#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#define ENTER 13
#define ESC 27
#define SPACEBAR 32

//===========================================================================================================================

char title[] = "Saya sedih sekali";
int res_x = 1366;
int res_y = 768;
float fov = 60.0f;
int is_fullscreen = 0;
int refreshRate = 1000/60;
// WASD controls
int up = 0;
int down = 0;
int left = 0;
int right = 0;
int jump = 0;
int crouch = 0;
int sprint = 0;
int flying = 0;
// angle of rotation for the camera direction
int xOrigin = 683;
int yOrigin = 384;
float angle_x = 0.0f;
float angle_y = 0.0f;
float deltaAngle_x = 0.0f;
float deltaAngle_y = 0.0f;
// current position of the camera
float x=0.0f, y=1.8f, z=5.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;

//===========================================================================================================================

void screenResize(int w, int h);
void toggle_fullscreen();
void render3D(void);
void drawSnowMan();
void key_press(unsigned char key, int xx, int yy);
void key_release(unsigned char key, int x, int y);
void specKey_press(int key, int xx, int yy);
void specKey_release(int key, int x, int y);
void move();
void camera(int x, int y);
void mouseButton(int button, int state, int x, int y);

//===========================================================================================================================

int main(int argc, char **argv) {

	// init GLUT and create window
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
	glutWarpPointer(res_x/2, res_y/2);
	move();
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(	x, y, z,
			x+lx, y+ly, z+lz,
			0.0f, 1.0f, 0.0f);
			
// Draw ground
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();

// Draw 36 SnowMen
	int i,j;
	for(i = -3; i < 3; i++)
		for(j=-3; j < 3; j++) {
                     glPushMatrix();
                     glTranslatef(i*10.0,0,j * 10.0);
                     drawSnowMan();
                     glPopMatrix();
               }
        glutSwapBuffers();
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
	mod = 0;
    switch(key){
    	case ESC:
    		exit(0);
    		break;
		case 'w':
			up = 1;
			break;
		case 'a':
			left = 1;
			break;
		case 's':
			down = 1;
			break;
		case 'd':
			right = 1;
			break;
		case 'c':
			crouch = 1;
			break;
		case SPACEBAR:
			jump = 1;
			break;
	}
}

void key_release(unsigned char key, int x, int y){
	switch(key){
		case 'w':
			up = 0;
			sprint = 1;
			break;
		case 'a':
			left = 0;
			break;
		case 's':
			down = 0;
			break;
		case 'd':
			right = 0;
			break;
		case 'c':
			crouch = 0;
			break;
	}
}

void specKey_press(int key, int xx, int yy) {

    switch (key) {
    	
    }
} 

void specKey_release(int key, int x, int y) { 	

    switch (key) {
    	
    }
} 

void move(){	
	if(up && !down){
		x += 0.03f * lx;
		z += 0.03f * lz;
		if(flying == 1){
			y += 0.03f * ly;
		}
	}
	if(!up && down){
		x -= 0.03f * lx;
		z -= 0.03f * lz;
		if(flying == 1){
			y -= 0.03f * ly;
		}
	}
	if(left && !right){
		x += 0.03f * lz;
		z -= 0.03f * lx;
	}
	if(!left && right){
		x -= 0.03f * lz;
		z += 0.03f * lx;
	}
	if(jump){
		if(y<3.0f){
			y += 0.03f/(9.8*9.8/60);
		}else if(y>=3.0f){
			jump = 0;
		}
	}
	if(!jump && y>1.8f && !flying){
		y -= 0.03f/(9.8*9.8/60);
	}
	if(crouch){
		if(y>1.0f){
			y -= 0.03f/(9.8*9.8/60);
		}
	}
}

void camera(int x, int y) {
	
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

}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON) {
		if (state != GLUT_UP) {

		}
		else if (state == GLUT_UP) {

		}
	}
}

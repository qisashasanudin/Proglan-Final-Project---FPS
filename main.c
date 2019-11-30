#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

char title[] = "Saya sedih sekali";
float anglePyramid = 0.0f;  // Rotational angle for pyramid [NEW]
float angleCube = 0.0f;     // Rotational angle for cube [NEW]
int refreshRate = 1000/60;        // refresh interval in milliseconds [NEW]

int up = 0;
int down = 0;
int left = 0;
int right = 0;

// angle of rotation for the camera direction
float angle_x=0.0f;
float angle_y=0.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;
// current position of the camera
float x=0.0f, y=1.0f, z=5.0f;

float deltaAngle_x = 0.0f;
float deltaAngle_y = 0.0f;
float deltaMove_fwd = 0.0f;
float deltaMove_side = 0.0f;

int xMouseState = -1;
int yMouseState = -1;

//===========================================================================================================================

void screenResize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
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

void walk() {
	x += deltaMove_fwd * lx * 0.1f;
	z += deltaMove_fwd * lz * 0.1f;
}

void strafe() {
	z += deltaMove_side * lx * 0.1f;
	x += deltaMove_side * lz * 0.1f;
}

void renderScene(void) {
	if (deltaMove_fwd)
		walk();
	if (deltaMove_side)
		strafe();
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(	x, y, z,
			x+lx, y+ly, z+lz,
			0.0f, 1.0f, 0.0f);
		printf("%f %f %f\n", x, y, z);
			
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

void key_press(unsigned char key, int xx, int yy) { 	
    switch(key){
    	case 27:
    		exit(0);
    		break;
		case 'w':
			up = 1;
			deltaMove_fwd = 0.5f;
			break;
		case 'a':
			left = 1;
			deltaMove_side = 0.5f;
			break;
		case 's':
			down = 1;
			deltaMove_fwd = -0.5f;
			break;
		case 'd':
			right = 1;
			deltaMove_side = -0.5f;
			break;
	}
}

void key_release(unsigned char key, int x, int y)
{
	switch(key){
		case 'w':
			up = 0;
			deltaMove_fwd = 0.0f;
			break;
		case 'a':
			left = 0;
			deltaMove_side = 0.0f;
			break;
		case 's':
			down = 0;
			deltaMove_fwd = 0.0f;
			break;
		case 'd':
			right = 0;
			deltaMove_side = 0.0f;
			break;
	}
}

void specKey_press(int key, int xx, int yy) {

       switch (key) {
             case GLUT_KEY_UP : deltaMove_fwd = 0.5f; break;
             case GLUT_KEY_DOWN : deltaMove_fwd = -0.5f; break;
       }
} 

void specKey_release(int key, int x, int y) { 	

        switch (key) {
             case GLUT_KEY_UP :
             case GLUT_KEY_DOWN : deltaMove_fwd = 0.0f;break;
        }
} 

void mouseMove(int x, int y) { 	
    if (xMouseState >= 0) {
		// update deltaAngle
		deltaAngle_x = (x - xMouseState) * 0.001f;
		// update camera's direction
		lx = sin(angle_x + deltaAngle_x);
		lz = -cos(angle_x + deltaAngle_x);
	}
	if (yMouseState >= 0) {
		// update deltaAngle
		deltaAngle_y = (y - yMouseState) * 0.001f;
		// update camera's direction
		ly = sin(angle_y + deltaAngle_y);
	}
}

void mouseButton(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {
		if (state != GLUT_UP) {
			xMouseState = x;
			yMouseState = y;
		}
		else if (state == GLUT_UP) {
			angle_x += deltaAngle_x;
			angle_y += deltaAngle_y;
			
			if(angle_y > 1.0f){
				angle_y = 1.0f;
			}else if(angle_y < -1.0f){
				angle_y = -1.0f;
			}
			
			xMouseState = -1.0f;
			yMouseState = -1.0f;
		}
	}
}

//===========================================================================================================================

int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1366,768);
	glutCreateWindow("Lighthouse3D - GLUT Tutorial");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(screenResize);
	glutIdleFunc(renderScene);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(key_press);
	glutKeyboardUpFunc(key_release);
	glutSpecialFunc(specKey_press);
	glutSpecialUpFunc(specKey_release);

	// here are the two new functions
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

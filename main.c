#include <stdio.h>
#include <windows.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>


char image[] = "resources/kucing.jpg";
char title[] = "Saya sedih sekali";
float anglePyramid = 0;  // Rotational angle for pyramid [NEW]
float angleCube = 0;     // Rotational angle for cube [NEW]
int refreshMills = 1000/60;        // refresh interval in milliseconds [NEW]
float cx = 0, cy = 0, cz = -7;

int up = 0;
int down = 0;
int left = 0;
int right = 0;
float x_vel = 0;
float y_vel = 0;

// angle of rotation for the camera direction
float angle=0.0;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;
// XZ position of the camera
float x=0.0f, y=1.0f, z=5.0f;

float deltaAngle = 0.0f;
float deltaMove = 0;

//===========================================================================================================================

void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void reshape(int w, int h) {
	if(h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45,ratio,1,100);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard_down(unsigned char key, int x, int y)
{
	switch(key){
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
	}
}

void keyboard_up(unsigned char key, int x, int y)
{
	switch(key){
		case 'w':
			up = 0;
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
	}
}
 
void idle(){
	glutPostRedisplay();
}
 
/* Called back when timer expired [NEW] */
void timer(int value) {
   glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}

void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
 
   // Render a color-cube consisting of 6 quads with different colors
   glLoadIdentity();                 // Reset the model-view matrix
   glTranslatef(0, 0, -7.0f);  // Move right and into the screen
   glRotatef(angleCube, 1, 1, 0);  // Rotate about (1,1,1)-axis [NEW]
   glRotatef(angleCube, 1, 0, 1);
   glRotatef(angleCube, 0, 1, 1);
 
   glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
      // Top face (y = 1.0f)
      // Define vertices in counter-clockwise (CCW) order with normal pointing out
      glColor3f(0.0f, 1.0f, 0.0f);     // Green
      glVertex3f( 1.0f, 1.0f, -1.0f);
      glVertex3f(-1.0f, 1.0f, -1.0f);
      glVertex3f(-1.0f, 1.0f,  1.0f);
      glVertex3f( 1.0f, 1.0f,  1.0f);
 
      // Bottom face (y = -1.0f)
      glColor3f(1.0f, 0.5f, 0.0f);     // Orange
      glVertex3f( 1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f( 1.0f, -1.0f, -1.0f);
 
      // Front face  (z = 1.0f)
      glColor3f(1.0f, 0.0f, 0.0f);     // Red
      glVertex3f( 1.0f,  1.0f, 1.0f);
      glVertex3f(-1.0f,  1.0f, 1.0f);
      glVertex3f(-1.0f, -1.0f, 1.0f);
      glVertex3f( 1.0f, -1.0f, 1.0f);
 
      // Back face (z = -1.0f)
      glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
      glVertex3f( 1.0f, -1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f( 1.0f,  1.0f, -1.0f);
 
      // Left face (x = -1.0f)
      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
      glVertex3f(-1.0f,  1.0f,  1.0f);
      glVertex3f(-1.0f,  1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f(-1.0f, -1.0f,  1.0f);
 
      // Right face (x = 1.0f)
      glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
      glVertex3f(1.0f,  1.0f, -1.0f);
      glVertex3f(1.0f,  1.0f,  1.0f);
      glVertex3f(1.0f, -1.0f,  1.0f);
      glVertex3f(1.0f, -1.0f, -1.0f);
   glEnd();  // End of drawing color-cube
 
   glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
 
   // Update the rotational angle after each refresh [NEW]
   angleCube -= 0.1;
}

//===========================================================================================================================

int main(int argc, char** argv) {
   glutInit(&argc, argv);            // Initialize GLUT
   glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
   glutInitWindowSize(1280, 720);   // Set the window's initial width & height
   glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
   glutCreateWindow(title);          // Create window with the given title
   glutDisplayFunc(display);       // Register callback handler for window re-paint event
   glutReshapeFunc(reshape);       // Register callback handler for window re-size event
   glutKeyboardFunc(keyboard_down);
   glutKeyboardUpFunc(keyboard_up);
   glutIdleFunc(idle);
   initGL();                       // Our own OpenGL initialization
   glutTimerFunc(0, timer, 0);     // First timer call immediately [NEW]
   glutMainLoop();                 // Enter the infinite event-processing loop
	
   return 0;
}

#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)
#define SPEED (300)

//===========================================================================================================================

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
int close_requested = 0;

int mouse_x, mouse_y, mouse_buttons, target_x, target_y;
float delta_x, delta_y, distance;

int initSDL(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}
}

void keyboard_input(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT:
				close_requested = 1;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode){
					case SDL_SCANCODE_W:
						up = 1;
						break;
					case SDL_SCANCODE_A:
						left = 1;
						break;
					case SDL_SCANCODE_S:
						down = 1;
						break;
					case SDL_SCANCODE_D:
						right = 1;
						break;
				}
				break;
			case SDL_KEYUP:
				switch(event.key.keysym.scancode){
					case SDL_SCANCODE_W:
						up = 0;
						break;
					case SDL_SCANCODE_A:
						left = 0;
						break;
					case SDL_SCANCODE_S:
						down = 0;
						break;
					case SDL_SCANCODE_D:
						right = 0;
						break;
				}
				break;
		}
	}
}	

void mouse_input(){
	mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
}

void windowSDL(){
	SDL_Window *win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	
	SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);
	SDL_Surface *surface = IMG_Load(image);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);
	
	SDL_Rect dest;
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
	dest.w /= 4;
	dest.h /= 4;
	
	float x_pos = (WINDOW_WIDTH - dest.w) /2;
	float y_pos = (WINDOW_HEIGHT - dest.h) /2;
	float x_vel = 0;
	float y_vel = 0;
	
	while(!close_requested){
		keyboard_input();
		/*	
		x_vel = y_vel = 0;
		if(up && !down) y_vel = -SPEED;
		if(down && !up) y_vel = SPEED;
		if(left && !right) x_vel = -SPEED;
		if(right && !left) x_vel = SPEED;
		*/
		
		mouse_input();	
		target_x = mouse_x - dest.w/2;
		target_y = mouse_y - dest.h/2;
		delta_x = target_x - x_pos;
		delta_y = target_y - y_pos;
		distance = sqrt(delta_x*delta_x + delta_y*delta_y);
		if(distance<5){
			x_vel = y_vel = 0;
		}else{
			x_vel = delta_x * SPEED / distance;
			y_vel = delta_y * SPEED / distance;
		}
		
		x_pos += x_vel/60;
		y_pos += y_vel/60;
		
		if(x_pos <= 0){x_pos = 0;}
		if(y_pos <= 0){y_pos = 0;		}
		if(x_pos >= WINDOW_WIDTH - dest.w){x_pos = WINDOW_WIDTH - dest.w;}
		if(y_pos >= WINDOW_HEIGHT - dest.h){y_pos = WINDOW_HEIGHT - dest.h;}
		
		dest.x = (int) x_pos;
		dest.y = (int) y_pos;
		
		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, tex, NULL, &dest);
		SDL_RenderPresent(rend);
		
		SDL_Delay(1000/60);
	}
	
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
}

//===========================================================================================================================

void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}
	
void keyboard(unsigned char key, int x, int y) {
	initSDL();
	keyboard_input();
 
 	 if (up && !down) {
	  cz += 0.1f;
	 }
	
	 if (down && !up) {
	  cz -= 0.1f;
	 }
	
	 if (left && !right) {
	  cx += 0.1f;
	 }
	
	 if (right && !left) {
	  cx -= 0.1f;
	 }
}

void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

   glLoadIdentity();                 // Reset the model-view matrix
   glTranslatef(cx, cy, cz);  // Move right and into the screen
   glRotatef(angleCube, 0, 5, 0);  // Rotate about (1,1,1)-axis [NEW]
 
   glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
      // Top face (y = 1.0f)
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
}
 
/* Called back when timer expired [NEW] */
void timer(int value) {
   glutPostRedisplay();      // Post re-paint request to activate display()
   glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   gluPerspective(40, aspect, 0.1, 100);
}


//===========================================================================================================================


int main(int argc, char** argv) {
	
	initSDL();
	windowSDL();
	
   glutInit(&argc, argv);            // Initialize GLUT
   glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
   glutInitWindowSize(1280, 720);   // Set the window's initial width & height
   glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
   glutCreateWindow(title);          // Create window with the given title
   glutDisplayFunc(display);       // Register callback handler for window re-paint event
   glutReshapeFunc(reshape);       // Register callback handler for window re-size event
   glutKeyboardFunc(keyboard);
   initGL();                       // Our own OpenGL initialization
   glutTimerFunc(0, timer, 0);     // First timer call immediately [NEW]
   glutMainLoop();                 // Enter the infinite event-processing loop
	
   return 0;
}

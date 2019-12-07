#ifndef _MAIN_H
#define _MAIN_H


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "data/lib/jpeg.lib")

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library




//////////////////////////////////////
//The Global Variables
//////////////////////////////////////
extern	HDC			hDC;			// Device Context
extern	HGLRC		hRC;			// Permanent Rendering Context
extern	HWND		hWnd;			// Holds Our Window Handle
extern	HINSTANCE	hInstance;		// Holds The Instance Of The Application


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


#endif

//Ronny André Reierstad
//www.morrowland.com
//apron@morrowland.com

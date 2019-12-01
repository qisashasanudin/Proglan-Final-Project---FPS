#include <stdio.h>
#include <windows.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)
#define SPEED (500)

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
float x_vel = 0;
float y_vel = 0;

int initSDL(){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}
}

void keyboardSDL(){
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
		x_vel = y_vel = 0;
		if(up && !down) y_vel = -SPEED;
		if(down && !up) y_vel = SPEED;
		if(left && !right) x_vel = -SPEED;
		if(right && !left) x_vel = SPEED;
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
	
	while(!close_requested){
		keyboardSDL();		
		
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


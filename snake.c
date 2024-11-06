/* ============================================================================
	SNAKE.CPP: SDL-based Simple Snake Game
	----------------------------------------------------------------------------
	Version 0.3
	Coded by Trinh D.D. Nguyen
	Dependencies: SDL2 (libsdl.org)
	Tested on:
	OS              Compiler      Hardware
	--------------  ------------  --------------------------
	. Win10         Dev-C++       Intel HD Graphics
	. Win10         Dev-C++       NVIDIA GTX 1060 6GB
	. Win10         MinGW64 LLVM  NVIDIA GTX 1060 6GB
	. macOS         Clang         Intel Iris Graphics 6100
	. Ubuntu 23.04  GCC           i945GM
	----------------------------------------------------------------------------
	NOTE: Please make sure SDL2 has already installed and properly configured.
	----------------------------------------------------------------------------
	Dev-C++ project: 'snake.dev'.
	Using make: type 'make' to build the game
	Manually compiling: type 'gcc -O3 snake.c -o snake -lSDL2' to compile
	----------------------------------------------------------------------------
	Usage: Just type 'snake' to play  
	============================================================================*/

#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <SDL2/SDL.h>

#define GAME_VERSION	"0.4"
#define	GAME_SPEED		100
#define	WINDOW_WIDTH	640
#define	WINDOW_HEIGHT	480
#define	SNAKE_LENGTH	WINDOW_WIDTH * WINDOW_HEIGHT
#define	CELL_W			16
#define	CELL_H			16
#define	GRID_W			(WINDOW_WIDTH/CELL_W)
#define	GRID_H			(WINDOW_HEIGHT/CELL_H)

typedef enum {BODY, HEAD, FOOD} TYPE;
typedef enum {LEFT, RIGHT, UP, DOWN} DIR;
typedef enum {false, true} bool;

typedef struct vec2 {
	int x, y;
} VEC2;

typedef	struct cell {
	VEC2 coords;
	int r, g, b;
	TYPE t;
} CELL;

typedef struct snake {
	CELL body[SNAKE_LENGTH];
	int length;
	DIR d;
} SNAKE;

SDL_Window * g_wnd = NULL;
SDL_Renderer * g_rend = NULL;
bool g_full = false;
bool g_grid = false;
SNAKE snake;

void cell_draw(const CELL * c) {
	SDL_Rect rc = {c->coords.x*CELL_W, c->coords.y*CELL_W, CELL_W, CELL_H};
	int r, g, b;
	switch (c->t) {
	case HEAD: 	r = g = b = 0xff; break;
	case BODY: 	r = g = b = 0x7f; break;
	case FOOD:	r = c->r; g = c->g; b = c->b; break;
	}
	SDL_SetRenderDrawColor(g_rend, r, g, b, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(g_rend, &rc);
}

void frame_draw(int x, int y) {
	SDL_Rect rc = {x * CELL_W, y * CELL_W, CELL_W, CELL_H};
	SDL_SetRenderDrawColor(g_rend, 0x1f, 0x1f, 0x1f, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(g_rend, &rc);
}

void cell_init(CELL * c, int x, int y, TYPE t) {
	c->t = t; c->coords.x = x; c->coords.y = y;
	switch(t) {
	case HEAD: 	c->r = c->g = c->b = 0xff; 
			    break;
	case BODY: 	c->r = c->g = c->b = 0x7f; 
			    break;
	case FOOD:	c->r = 32 + rand() % 224;
			    c->g = 32 + rand() % 224; 
				c->b = 32 + rand() % 224; 
				break;
	}
}

bool cell_hit(const CELL * a, const CELL * b) {
	return (a->coords.x == b->coords.x && a->coords.y == b->coords.y);
}

void cell_copy_coords(CELL * dst, CELL * src) {
	dst->coords = src->coords;
	dst->r = src->r; dst->g = src->g; dst->b = src->b;
}

void food_init(CELL * f) {
	cell_init(f, rand() % GRID_W, rand() % GRID_H, FOOD);
}

bool snake_init() {
	char title[256];

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Error: %s\n", SDL_GetError());
		return false;
	}

	sprintf(title, "Snake %s - Trinh D.D. Nguyen", GAME_VERSION);
	g_wnd = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
									WINDOW_WIDTH,  WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!g_wnd) {
		fprintf(stderr, "Error: %s\n", SDL_GetError());
		return false;
	}

	g_rend = SDL_CreateRenderer(g_wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	if(!g_rend) {
		fprintf(stderr, "Error: %s\n", SDL_GetError());
		return false;
	}

	memset(&snake, 0, sizeof(snake));

	snake.length = 2;
	cell_init(&snake.body[0], GRID_W/2, GRID_H/2, HEAD);
	cell_init(&snake.body[1], GRID_W/2, GRID_H/2+1, BODY);
	snake.d = UP;

	return true;
}

void snake_done() {
	if (g_full) {
		SDL_SetWindowFullscreen(g_wnd, false);
		SDL_ShowCursor(true);
	}
	SDL_DestroyWindow(g_wnd);
	SDL_Quit();
}

void snake_draw() {
	int i;
	for (i = 0; i < snake.length; i++)
		cell_draw(&snake.body[i]);
}

void snake_exec() {
	SDL_Event e;
	bool quit = false;
	int i, j;
	CELL food;

	fprintf(stderr, "- Controls...\n"
	                "  WASD - Move\n"
					"  SPACE - Toggle full screen\n"
					"  ESC - Quit\n");
	food_init(&food);

	while( quit == false ){
		while( SDL_PollEvent( &e ) ){
			if( e.type == SDL_QUIT ) quit = true;
			else if( e.type == SDL_KEYDOWN ) {
                switch( e.key.keysym.sym )
                {
                case SDLK_w:
                case SDLK_UP: 	if (snake.d != DOWN)  snake.d = UP; break;
                case SDLK_s:
                case SDLK_DOWN:	if (snake.d != UP)    snake.d = DOWN; break;
                case SDLK_a:
                case SDLK_LEFT:	if (snake.d != RIGHT) snake.d = LEFT; break;
                case SDLK_d:
                case SDLK_RIGHT:if (snake.d != LEFT)  snake.d = RIGHT; break;
                case SDLK_g:
                	g_grid = !g_grid;
                	break;
				case SDLK_ESCAPE: quit = true; break;
				case SDLK_SPACE:
					g_full = !g_full;
					SDL_SetWindowFullscreen(g_wnd, g_full);
					SDL_ShowCursor(!g_full);
					break;
                default: break;
                }
			}
		}

		SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 0xFF );
		SDL_RenderClear(g_rend);

		for (i = snake.length-1; i > 0; i--) {
			cell_copy_coords(&snake.body[i], &snake.body[i-1]);
			snake.body[i].t = BODY;
		}
		snake.body[0].t = HEAD;

		switch(snake.d) {
		case UP   : snake.body[0].coords.y--; break;
		case DOWN : snake.body[0].coords.y++; break;
		case LEFT : snake.body[0].coords.x--; break;
		case RIGHT: snake.body[0].coords.x++; break;
		}

		if (snake.body[0].coords.y < 0)         snake.body[0].coords.y = GRID_H-1;
		if (snake.body[0].coords.y > GRID_H-1)	snake.body[0].coords.y = 0;
		if (snake.body[0].coords.x < 0)        	snake.body[0].coords.x = GRID_W-1;
		if (snake.body[0].coords.x > GRID_W-1) 	snake.body[0].coords.x = 0;

		if (g_grid) {
			for (i = 0; i < GRID_W; i++)
			for (j = 0; j < GRID_H; j++)
				frame_draw(i, j);
		}

		cell_draw(&food);
		snake_draw();

		if (cell_hit(&snake.body[0], &food)) {
			snake.length++;
			food_init(&food);
		}

		SDL_Delay(GAME_SPEED);
		SDL_RenderPresent( g_rend );
	}
}

int main(int argc, char** argv) {
	if (!snake_init()) {
		printf("Cannot initialize game.\n");
		return -1;
	}
	snake_exec();
	snake_done();
	return 0;
}

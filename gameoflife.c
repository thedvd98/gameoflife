#include <stdio.h>
#include <SDL2/SDL.h>

#define WIN_WIDTH 640
#define WIN_HEIGHT 640 

#define RECT_WIDTH 10
#define RECT_HEIGHT 10

#define DELAY 100

#define CELL_DEAD 0
#define CELL_ALIVE 1
#define CELL_TO_DIE 2
#define CELL_TO_RESSURECT 3

SDL_Window *win;
SDL_Renderer *renderer;

void drawRect(int x, int y) {
	SDL_Rect rect = {x, y, RECT_WIDTH, RECT_HEIGHT};
	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect( renderer, &rect);
}

void drawGrid(int **grid, int nw, int nh) {
	int mult_x = RECT_WIDTH;
	int mult_y = RECT_HEIGHT;
	SDL_SetRenderDrawColor(renderer, 119, 119, 119, 119);
	for (int x = 0; x < nw; x++) {
		SDL_RenderDrawLine(renderer, x*mult_x, 0, x*mult_x, WIN_HEIGHT);
	}
	for(int y = 0; y < nh; y++) { 
		SDL_RenderDrawLine(renderer, 0, y*mult_y, WIN_WIDTH, y*mult_y);
	}

	for (int i = 0; i < nw; i++) {
		for(int j = 0; j < nh; j++) {
			if(grid[i][j] == CELL_ALIVE){
				drawRect(i*mult_x, j*mult_y);
			}
		}
	}

}

void clicked(int **grid, int x, int y) {
	int i, j;
	i = x/RECT_WIDTH;
	j = y/RECT_HEIGHT;

	printf("Clicked left [%d, %d] at cell -> [%d, %d]\n", x, y, i, j);
	grid[i][j] = CELL_ALIVE;

}

void grid_init(int **grid, int nw, int nh) {
	for (int i = 0; i < nw; i++) {
		for(int j = 0; j < nh; j++) {
			grid[i][j] = CELL_DEAD;
		}
	}
}

void copyGrid(int **src_grid, int **dst_grid, int nw, int nh) {
	for (int i = 0; i < nw; i++) {
		for (int j = 0; j < nh; j++) {
			dst_grid[i][j] = src_grid[i][j];
		}
	}
}

int getCellStatus(int **grid, int nw, int nh, int i, int j){

	if(i < 0 || i >= nw) {
		return CELL_DEAD;
	}
	if(j < 0 || j >= nh) {
		return CELL_DEAD;
	}
	if (grid[i][j] == CELL_ALIVE || grid[i][j] == CELL_TO_DIE) {
		return CELL_ALIVE;
	}

	return CELL_DEAD;
}


void howManyLiveCellsAround(int **grid, int nw, int nh, int i, int j) {
	int n_alive = 0;
	n_alive += getCellStatus(grid, nw, nh, i, j+1);
	n_alive += getCellStatus(grid, nw, nh, i+1, j);
	n_alive += getCellStatus(grid, nw, nh, i+1, j+1);
	n_alive += getCellStatus(grid, nw, nh, i-1, j-1);
	n_alive += getCellStatus(grid, nw, nh, i, j-1);
	n_alive += getCellStatus(grid, nw, nh, i-1, j);
	n_alive += getCellStatus(grid, nw, nh, i-1, j+1);
	n_alive += getCellStatus(grid, nw, nh, i+1, j-1);

	switch(getCellStatus(grid, nw, nh, i, j)) {
		case CELL_ALIVE:
			if (n_alive < 2){
				grid[i][j] = CELL_TO_DIE;
				return;
			} else if (n_alive > 3) {
				grid[i][j] = CELL_TO_DIE;
				return;
			} else {
				grid[i][j] = CELL_ALIVE;
			}
			break;
		case CELL_DEAD:
			if(n_alive == 3) {
				grid[i][j] = CELL_TO_RESSURECT;
			}
			return;
			break;
		default:
			grid[i][j] = CELL_TO_DIE;
			break;
	}
}

void applyRules(int **grid, int nw, int nh) {
	for (int i = 0; i < nw; i++) {
		for(int j = 0; j < nh; j++) {
			if (grid[i][j] == CELL_TO_DIE)
				grid[i][j] = CELL_DEAD;
			if (grid[i][j] == CELL_TO_RESSURECT)
				grid[i][j] = CELL_ALIVE;
		}
	}
	for (int i = 0; i < nw; i++) {
		for(int j = 0; j < nh; j++) {
			howManyLiveCellsAround(grid, nw, nh, i, j);
		}
	}
}


void gameLoop() {

	SDL_Event event;
	int continua = 1;
	int gamePaused = 0;

	int nw = WIN_WIDTH / 2;
	int nh = WIN_HEIGHT / 2;
	printf("grid[%d][%d]\n",nw, nh);

	int mousex;
	int mousey;

	int *grid[nw];
	for (int i = 0; i < nw; i++)
		grid[i] = malloc(nh * sizeof(int));

	grid_init(grid, nw, nh);

	while(continua) {
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				continua = 0;
			}
			switch(event.type) {
				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT){
						SDL_GetMouseState(&mousex, &mousey);
						clicked(grid, mousex, mousey);
					}
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_q:
							continua = 0;
						case SDLK_SPACE:
							gamePaused = !gamePaused;
							printf("Key clicked %d\n", gamePaused);
							break;
						case SDLK_c:
							grid_init(grid, nw, nh);
							break;
					}
					break;

			}
		}

		if(gamePaused) {
			applyRules(grid, nw, nh);
		}

		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		drawGrid(grid, nw, nh);

		SDL_RenderPresent(renderer);
		SDL_Delay(DELAY);
	}

	for (int i = 0; i < nw; i++)
		free(grid[i]);
}

int main(int argc, char **argv) {
	SDL_Surface *screenSurface;
	int flags = 0;
	char* titolo = "life";
	int xpos = 10;
	int ypos = 10;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("Errore nell'inizializzare SDL: %s\n", SDL_GetError());
		return 1;
	}
	win = SDL_CreateWindow(titolo, xpos, ypos, WIN_WIDTH, WIN_HEIGHT, flags);
	if (win == NULL) {
		printf("Errore nella creazione della finestra: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	//SDL_RENDERER_ACCELERATED
	renderer = SDL_CreateRenderer(win, -1, 0);
	if (renderer == NULL) {
		printf("Errore nella creazione del render: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	screenSurface = SDL_GetWindowSurface(win);
	gameLoop();

	SDL_FreeSurface(screenSurface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	win = NULL;
	renderer = NULL;
	SDL_Quit();
	return 0;
}

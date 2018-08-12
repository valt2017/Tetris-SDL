#include <iostream>
#include <fstream>
#include <SDL.h>
#include <map>
#include <vector> //for random shuffle only
#include <ctime> // for random shuffle only

// Ported by @valt to SDL from https://www.youtube.com/user/FamTrinli tetris tutorial in SFML

//figures coordinates
//std::map<int, SDL_Rect> Figures;
const int FigureSize = 18;
const int ItemsX = 20;
const int ItemsY = 10;
const int FPS = 60;
const int ANIMATION_DELAY = 5000;
// Tetris board
int tetris[ItemsX][ItemsY] = {0,};
std::vector<SDL_Texture *> Textures;
std::vector<std::string> Bitmaps;
const int posX = 100, posY = 100;
const int sizeX = 320, sizeY = 480;
SDL_Rect board = {0, 0, sizeX, sizeY};

struct Point
{
	int x, y;
} a[4], b[4];

int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

bool check()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= ItemsY || a[i].y >= ItemsX) 
			return 0;
		else if (tetris[a[i].y][a[i].x]) 
			return 0;
		return 1;
};


int main(int argc, char ** argv) {

	// Error checks
	std::cout << "SDL_Init\n";
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *win = SDL_CreateWindow("Puzzle", posX, posY, sizeX, sizeY, 0);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow error\n";
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer error\n";
	}
	// Load bitmaps
	Bitmaps.push_back("img/background.bmp");
	Bitmaps.push_back("img/tiles.bmp");
	// Create textures from bitmaps
	for (auto bitmap : Bitmaps) {
		SDL_Surface * bitmapSurface = SDL_LoadBMP(bitmap.c_str());
		// create texture 
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
		if (texture == nullptr) {
			std::cout << bitmap.c_str() <<" SDL_CreateTextureFromSurface error\n";
		}
		else {
			std::cout << bitmap.c_str() << " SDL_CreateTextureFromSurface OK\n";
			Textures.push_back(texture);
		}
		SDL_FreeSurface(bitmapSurface);
	}
	// Random shuffle
	srand(time(0));
	int dx = 0; bool rotate = 0; int colorNum = 1;
	float timer = 0, delay = 5.0;
	Uint32 start;

	while (1) {
		start = SDL_GetTicks();
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:
					dx = -1;
					break;
				case SDLK_RIGHT:
					dx = 1;
					break;
				case SDLK_UP:
					rotate = true;
					break;
				case SDLK_DOWN:
					delay = 0.5;
					break;
				default:
					std::cout << "Key not supported\n";
					break;
				}
			}
		}
		timer++;
		//// <- Move -> ///
		for (int i = 0; i < 4; i++) {
			b[i] = a[i]; 
			a[i].x += dx; 
		}
		if (!check()) 
			for (int i = 0; i < 4; i++) 
				a[i] = b[i];
		//////Rotate//////
		if (rotate)	{
			Point p = a[1]; //center of rotation
			for (int i = 0; i < 4; i++) {
				int x = a[i].y - p.y;
				int y = a[i].x - p.x;
				a[i].x = p.x - x;
				a[i].y = p.y + y;
			}
			if (!check()) 
				for (int i = 0; i<4; i++) 
					a[i] = b[i];
		}
		///////Tick//////
		if (timer > delay)
		{   // move down
			for (int i = 0; i<4; i++) { 
				b[i] = a[i]; 
				a[i].y += 1; 
			}
			if (!check()) {
				for (int i = 0; i < 4; i++) 
					tetris[b[i].y][b[i].x] = colorNum;
				colorNum = 1 + rand() % 7;
				int n = rand() % 7;
				for (int i = 0; i < 4; i++)	{
					a[i].x = figures[n][i] % 2 + 3;
					a[i].y = figures[n][i] / 2;
				}
			}
			timer = 0;
		}
		///////check lines//////////
		int k = ItemsX - 1;
		for (int i = ItemsX - 1; i > 0; i--) {
			int count = 0;
			for (int j = 0; j<ItemsY; j++) {
				if (tetris[i][j]) count++;
					tetris[k][j] = tetris[i][j];
			}
			if (count<ItemsY) 
				k--;
		}
		dx = 0; rotate = 0; delay = 5.0;
		SDL_RenderClear(renderer);
		auto texture = Textures[0];
		SDL_RenderCopy(renderer, texture, nullptr, &board);
		// bottom blocks texture
		texture = Textures[1];
		for (int i = 0; i < ItemsX; i++)
			for (int j = 0; j < ItemsY; j++) {
				if(tetris[i][j] != 0) {
					SDL_Rect DestR = { 28 + j*FigureSize, 31 + i*FigureSize, FigureSize, FigureSize };
					SDL_Rect SourceR = { tetris[i][j]*FigureSize, 0, FigureSize, FigureSize };
					SDL_RenderCopy(renderer, texture, &SourceR, &DestR);
				}
			}
		// moving block texture
		for (int i = 0; i < 4; i++)	{
			SDL_Rect DestR = { 28 + a[i].x * FigureSize, 31 + a[i].y * FigureSize, FigureSize, FigureSize };
			SDL_Rect SourceR = { colorNum * FigureSize, 0, FigureSize, FigureSize };
			SDL_RenderCopy(renderer, texture, &SourceR, &DestR);
		}
		SDL_RenderPresent(renderer);
		// Animation
		if (ANIMATION_DELAY / FPS > SDL_GetTicks() - start)
			SDL_Delay(ANIMATION_DELAY / FPS - (SDL_GetTicks() - start));
	}
	for (auto texture : Textures) 
		SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	std::cout << "SDL_Quit\n";
	return 0;
}
#pragma once

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )


#include "Entity.h"

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	360
#define MAX_KEYS		256
#define MAX_SHOTS		1
#define RED				0
#define YELLOW			1
#define BLUE			2

class Game
{
public:
	Game();
	~Game();

	bool Init();
	bool LoadImages();
	void Release();
	
	bool Input();
	bool Update();
	void Draw();

private:
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	SDL_Texture *img_background, *img_background2, *img_player, *img_food1, * img_food2, * img_food3, *img_customer1, * img_customer2, * img_customer3 /*If there is more than one model of costumers this can be an array*/;

	Entity Player, Shots[MAX_SHOTS], Scene, Food, Food2, Food3, Charge, FoodM[3], RedScreen, GreenScreen,
		TotalCustomers[60]//This is the total customers of the game
		;
	int maxCostumers = 3; //This is the maximum of costumers on screen (1 for each row)
	int idx_shot;
	int Command[3];
	int Counter = 0;
	
	float width;

	bool god_mode;

	enum KEY_STATE { KEY_IDLE, KEY_DOWN, KEY_REPEAT, KEY_UP	};
	KEY_STATE keys[MAX_KEYS]; 
};

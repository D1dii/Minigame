#include "Game.h"
#include <math.h>
#include <time.h>
#include<stdlib.h>


Game::Game() {}
Game::~Game(){}

bool Game::Init()
{
	srand(time(NULL));
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create our window: title, x, y, w, h, flags
	Window = SDL_CreateWindow("Spaceship: arrow keys + space, f1: god mode", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}
	//Create a 2D rendering context for a window: window, device index, flags
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		SDL_Log("Unable to create rendering context: %s", SDL_GetError());
		return false;
	}
	//Initialize keys array
	for (int i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;

	//Load images
	if (!LoadImages())
		return false;

	//Init variables
	//size: 104x82
	Player.Init(150, 384, 50, 50, 5);
	Charge.Init(150, Player.GetY() - 20, 10, 10, 5);
	Food.Init(75, 284, 50, 50, 0);
	Food2.Init(75, 384, 50, 50, 0);
	Food3.Init(75, 484, 50, 50, 0);
	for (int i = 0; i < maxCostumers; i++)
	{
		TotalCustomers[i].Init(600, 354 + (100 * (i - 1)), 50, 50, 0); 
		Command[i] = TotalCustomers[i].GetCommand();
	}
	idx_shot = 0;
	int w;
	SDL_QueryTexture(img_background, NULL, NULL, &w, NULL);
	Scene.Init(0, 0, w, WINDOW_HEIGHT, 4);
	god_mode = false;

	return true;
}
bool Game::LoadImages()
{
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_Log("IMG_Init, failed to init required png support: %s\n", IMG_GetError());
		return false;
	}
	img_background = SDL_CreateTextureFromSurface(Renderer, IMG_Load("background.png"));
	if (img_background == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_player = SDL_CreateTextureFromSurface(Renderer, IMG_Load("spaceship.png"));
	if (img_player == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_shot = SDL_CreateTextureFromSurface(Renderer, IMG_Load("shot.png"));
	if (img_shot == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_customer= SDL_CreateTextureFromSurface(Renderer, IMG_Load("shot.png"));
	if (img_shot == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	return true;
}
void Game::Release()
{
	//Release images
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_player);
	SDL_DestroyTexture(img_shot);
	SDL_DestroyTexture(img_customer);
	IMG_Quit();
	
	//Clean up all SDL initialized subsystems
	SDL_Quit();
}
bool Game::Input()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)	return false;
	}

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}
bool Game::Update()
{
	//Read Input
	if (!Input())	return true;

	//Process Input
	int fx = 0, fy = 0, FoodY = 0, launchx = 0, FoodID = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_F1] == KEY_DOWN)		god_mode = !god_mode;
	if (Player.GetY() >= 364) {
		if (keys[SDL_SCANCODE_UP] == KEY_DOWN)			fy = -20;
	}
	if (Player.GetY() <= 404) {
		if (keys[SDL_SCANCODE_DOWN] == KEY_DOWN)	fy = 20;
	}
	if (keys[SDL_SCANCODE_SPACE] == KEY_REPEAT)
	{
		width = width + 0.25f;
		if (width >= 50) {
			width = 0;
      			FoodY = Player.GetY();
		}
	}
	else {
		width = width - 0.25f;
		if (width <= 0) {
			width = 0;
		}
	}
	if (keys[SDL_SCANCODE_M] == KEY_REPEAT) {
		launchx = 1;
	}

	//Logic
	//Scene scroll
	Scene.Move(-1, 0);
	if (Scene.GetX() <= -Scene.GetWidth())	Scene.SetX(0);
	//Player update
	Player.Move(fx, fy);
	Charge.Move(fx, fy);
	FoodM[0].Move(fx, fy);
	FoodM[1].Move(fx, fy);
	FoodM[2].Move(fx, fy);

	//Disable Speed at the end

	if (FoodM[0].GetX() == 500) {
		FoodM[0].SetSpeed(0);
	}
	if (FoodM[1].GetX() == 500) {
		FoodM[1].SetSpeed(0);
	}
	if (FoodM[2].GetX() == 500) {
		FoodM[2].SetSpeed(0);
	}
	
	//Get Food

	if (FoodY == Food.GetY()) {
		FoodM[0].Init(150, Player.GetY() - 30, 50, 50, 5);
		FoodID = RED;
	}
	else if (FoodY == Food2.GetY()) {
		FoodM[1].Init(150, Player.GetY() - 30, 50, 50, 5);
		FoodID = YELLOW;
	}
	else if (FoodY == Food3.GetY()) {
		FoodM[2].Init(150, Player.GetY() - 30, 50, 50, 5);
		FoodID = BLUE;
	}

	//Launch Food

	for (int i = 0; i < MAX_SHOTS; ++i) {
		if (FoodM[0].GetX() <= 500) FoodM[0].Move(launchx, 0);
	}

	for (int i = 0; i < MAX_SHOTS; ++i) {
		if (FoodM[1].GetX() <= 500) FoodM[1].Move(launchx, 0);
	}

	for (int i = 0; i < MAX_SHOTS; ++i) {
		if (FoodM[2].GetX() <= 500) FoodM[2].Move(launchx, 0);
	}

	//Customers update
	for (int i = 0; i < maxCostumers; i++)
	{
		TotalCustomers[i].Move(-1, 0);
	}

	//Command
	for (int i = 0; i < 3; ++i) {
		if (FoodM[i].GetX() >= 500) {
			int FoodMY = 0;
			FoodID = i;
			if (FoodM[i].GetY() == 254) {
				FoodMY = 0;
			}
			else if (FoodM[i].GetY() == 354) {
				FoodMY = 1;
			}
			else if (FoodM[i].GetY() == 454) {
				FoodMY = 2;
			}
			if (FoodID == Command[FoodMY]) {
				FoodM[i].Move(200, 0);
			}
		}
	}
	

	return false;
}
void Game::Draw()
{
	SDL_Rect rc;
	const int bar = width;

	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);

	//God mode uses red wireframe rectangles for physical objects
	if (god_mode) SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);

	//Draw scene
	Scene.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_background, NULL, &rc);
	rc.x += rc.w;
	SDL_RenderCopy(Renderer, img_background, NULL, &rc);

	//Draw player
	Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 150, 0, 150, 255);
	SDL_RenderFillRect(Renderer, &rc);
	if (god_mode) SDL_RenderDrawRect(Renderer, &rc);

	//Draw Charge
	Charge.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	rc.w = bar;
	SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(Renderer, &rc);

	//Draw Food
	Food.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 0);
	SDL_RenderFillRect(Renderer, &rc);

	Food2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(Renderer, &rc);

	Food3.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 0);
	SDL_RenderFillRect(Renderer, &rc);

	//Draw FoodM
	FoodM[0].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 0);
	SDL_RenderFillRect(Renderer, &rc);

	FoodM[1].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(Renderer, &rc);

	FoodM[2].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 0);
	SDL_RenderFillRect(Renderer, &rc);
	
	//Draw Customers 
	for (int i = 0; i < maxCostumers; ++i)
	{
		TotalCustomers[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0);
		SDL_RenderFillRect(Renderer, &rc);
		//SDL_RenderCopy(Renderer, img_shot, NULL, &rc);
		if (god_mode) SDL_RenderDrawRect(Renderer, &rc);

	}

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}
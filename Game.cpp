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
	Player.Init(90, 180, 50, 50, 5);
	Charge.Init(90, Player.GetY() - 20, 10, 10, 5);
	Food.Init(30, 105, 50, 0, 0);
	Food2.Init(30, 180, 50, 0, 0);
	Food3.Init(30, 255, 50, 0, 0);
	for (int i = 0; i < maxCostumers; i++)
	{
		TotalCustomers[i].Init(640, 150 + (75 * (i - 1)), 50, 50, rand() % 2 + 1);
		Command[i] = TotalCustomers[i].GetCommand();
	}
	idx_shot = 0;
	int w;
	SDL_QueryTexture(img_background, NULL, NULL, &w, NULL);
	SDL_QueryTexture(img_background2, NULL, NULL, &w, NULL);
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
	img_background = SDL_CreateTextureFromSurface(Renderer, IMG_Load("background1.png"));
	if (img_background == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_background2 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("background2.png"));
	if (img_background2 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_player = SDL_CreateTextureFromSurface(Renderer, IMG_Load("p.idling.png"));
	if (img_player == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_food1 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("lasana.png"));
	if (img_food1 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_food2 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("spaguetti.png"));
	if (img_food2 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_food3 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("pizza.png"));
	if (img_food3 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_customer1= SDL_CreateTextureFromSurface(Renderer, IMG_Load("cliente lasana.png"));
	if (img_customer1 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_customer2 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("cliente spaghetti.png"));
	if (img_customer2 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_customer3 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("cliente pizza.png"));
	if (img_customer3 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}

	return true;
}
void Game::Release()
{
	//Release images
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_background2);
	SDL_DestroyTexture(img_player);
	SDL_DestroyTexture(img_food1);
	SDL_DestroyTexture(img_food2);
	SDL_DestroyTexture(img_food3);
	SDL_DestroyTexture(img_customer1);
	SDL_DestroyTexture(img_customer2);
	SDL_DestroyTexture(img_customer3);
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
	if (Player.GetY() >= 165) {
		if (keys[SDL_SCANCODE_UP] == KEY_DOWN)			fy = -15;
	}
	if (Player.GetY() <= 195) {
		if (keys[SDL_SCANCODE_DOWN] == KEY_DOWN)	fy = 15;
	}
	if (keys[SDL_SCANCODE_SPACE] == KEY_REPEAT)
	{
		width = width + 0.35f;
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
	if (keys[SDL_SCANCODE_Z] == KEY_REPEAT) {
		launchx = 1;
	}

	//Logic
	
	//Player update
	Player.Move(fx, fy);
	Charge.Move(fx, fy);
	FoodM[0].Move(fx, fy);
	FoodM[1].Move(fx, fy);
	FoodM[2].Move(fx, fy);

	//Disable Speed at the end

	if (FoodM[0].GetX() == 155) {
		FoodM[0].SetSpeed(0);
	}
	if (FoodM[1].GetX() == 155) {
		FoodM[1].SetSpeed(0);
	}
	if (FoodM[2].GetX() == 155) {
		FoodM[2].SetSpeed(0);
	}
	
	//Get Food

	if (FoodY == Food.GetY()) {
		FoodM[0].Init(90, Player.GetY() - 30, 50, 50, 5);
		FoodID = RED;
		//Counter++;
	}
	else if (FoodY == Food2.GetY()) {
		FoodM[1].Init(90, Player.GetY() - 30, 50, 50, 5);
		FoodID = YELLOW;
		//Counter++;
	}
	else if (FoodY == Food3.GetY()) {
		FoodM[2].Init(90, Player.GetY() - 30, 50, 50, 5);
		FoodID = BLUE;
		//Counter++;
	}

	//Launch Food

	for (int i = 0; i < MAX_SHOTS; ++i) {
		if (FoodM[0].GetX() <= 155) FoodM[0].Move(launchx, 0);
	}

	for (int i = 0; i < MAX_SHOTS; ++i) {
		if (FoodM[1].GetX() <= 155) FoodM[1].Move(launchx, 0);
	}

	for (int i = 0; i < MAX_SHOTS; ++i) {
		if (FoodM[2].GetX() <= 155) FoodM[2].Move(launchx, 0);
	}

	//Customers update
	for (int i = 0; i < maxCostumers; i++)
	{
		if(TotalCustomers[i].GetX() > 210) TotalCustomers[i].Move(-1, 0);
	}

	//Command

	int FoodMY = 0;
	
	
	for (int i = 0; i < 3; ++i) {
		
		if (FoodM[i].GetX() >= 155) {
			
			FoodID = i;
			if (FoodM[i].GetY() == 75) {
				FoodMY = 0;
			}
			else if (FoodM[i].GetY() == 150) {
				FoodMY = 1;
			}
			else if (FoodM[i].GetY() == 225) {
				FoodMY = 2;
			}
			if (FoodID == Command[FoodMY]) {
				SDL_Delay(100);
				FoodM[i].Init(90, Player.GetY() - 30, 0, 0, 0);
				Command[FoodMY] = TotalCustomers[i].GetCommand();
				TotalCustomers[FoodMY].Init(640, 150 + (75 * (FoodMY - 1)), 50, 50, rand() % 2 + 1);
				Counter++;
				if (Counter == 5) {
					GreenScreen.Init(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
					SDL_Delay(250);
					return true;
				}
			}
			
			
		}
		
		
	}

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (FoodM[i].GetX() == 155 && FoodM[j].GetX() == 155 && i != j) {
				RedScreen.Init(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
				SDL_Delay(250);
				return true;
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

	Scene.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_background2, NULL, &rc);

	//Draw player
	Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_player, NULL, &rc);
	if (god_mode) SDL_RenderDrawRect(Renderer, &rc);

	//Draw Charge
	Charge.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	rc.w = bar;
	SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(Renderer, &rc);

	//Draw Food
	Food.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_food1, NULL, &rc);

	Food2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(Renderer, &rc);

	Food3.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 0);
	SDL_RenderFillRect(Renderer, &rc);

	//Draw FoodM
	FoodM[0].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_food1, NULL, &rc);

	FoodM[1].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_food2, NULL, &rc);

	FoodM[2].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_food3, NULL, &rc);
	
	//Draw Customers 
	for (int i = 0; i < maxCostumers; ++i)
	{
		TotalCustomers[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
		if (Command[i] == 0) {
			SDL_RenderCopy(Renderer, img_customer1, NULL, &rc);
		}
		else if (Command[i] == 1) {
			SDL_RenderCopy(Renderer, img_customer2, NULL, &rc);
		}
		else if (Command[i] == 2) {
			SDL_RenderCopy(Renderer, img_customer3, NULL, &rc);
		}
		
		if (god_mode) SDL_RenderDrawRect(Renderer, &rc);

	}

	//Draw Lose & Win Screen
	RedScreen.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 0);
	SDL_RenderFillRect(Renderer, &rc);

	GreenScreen.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 0);
	SDL_RenderFillRect(Renderer, &rc);

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}
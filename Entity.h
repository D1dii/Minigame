#pragma once

#define RED				0
#define YELLOW			1
#define BLUE			2

class Entity
{
public:
	Entity();
	~Entity();

	void Init(int posx, int posy, int w, int h, int s);
	void GetRect(int *posx, int *posy, int *w, int *h);
	void SetX(int posx);
	void SetSpeed(int vel);
	int	 GetCommand();
	int  GetX();
	int  GetY();
	int  GetWidth();
	void ShutDown();
	bool IsAlive();
	void Move(int dx, int dy);

private:
	int x, y;
	int width, height;
	int speed;
	bool is_alive;
};


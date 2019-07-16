#pragma once
#include "config.h"
#include <random>

class Game2048{
public:
	Game2048();
	int getStatus();
	void processInput();
	void draw();
	void quit();
private:
	int data[N][N];
	int status;
	int score;
	int hiscore;
	std::default_random_engine e;
	void start();
	bool randNew();
	bool moveLeft();
	void rotate();
	bool isOver();
};
#pragma once

#include <math.h>  

#include "map.h"
#include "pacman.h"
#include "ghost.h"
#include "map.h"

class Game
{
private:
public:
	bool paused;
	int coins;
	bool winConTriggered;
	bool lost;
	bool won;

	Pacman* pacman;
	vector<Ghost>* ghosts;
	GameMap* map;

	Game(GameMap* map, Pacman* pacman, vector<Ghost>* ghosts);
	void setMap(GameMap gm);
	void nextStep(int timeInterval);
	void checkCollision();
	void gameOver();
	void wonGame();
	int countCoins();
	bool isMidTile(pair<float, float> tile);
};


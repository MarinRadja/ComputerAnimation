#include "Game.h"


Game::Game(GameMap* map, Pacman* pacman, vector<Ghost>* ghosts) {
	this->paused = false;
	this->map = map;
	this->pacman = pacman;
	this->ghosts = ghosts;

	won = false;
	lost = false;
	winConTriggered = true;
}


void Game::nextStep(int timeInterval)
{
	(*pacman).updatePosition();
	for (int i = 0; i < ghosts->size(); i++) {
		ghosts->at(i).updatePosition();
	}

	if ((*pacman).powerUp == true) {
		(*pacman).powerUpDuration -= 1.f * timeInterval / 1000;
		if ((*pacman).powerUpDuration <= 0) {
			(*pacman).powerUp = false;
			(*pacman).currentSpeed = (*pacman).originalSpeed;
			for (int i = 0; i < ghosts->size(); i++) {
				ghosts->at(i).color = ghosts->at(i).originalColor;
			}
		}
	}
};

void Game::checkCollision() {
	// pacmanX and pacmanY are coordinates of pacman's left bottom corner
	float pacmanX = (*pacman).position.x, pacmanY = (*pacman).position.y;

	// check saved directions when pacman is in the mid of a tile
	if (pacman->direction != 0) {
		if (pacman->direction == UP && map->map[(int)round(pacmanX)][(int)round(pacmanY) + 1] != 1) {
			pacman->nextDirection = UP;
			pacman->direction = 0;
		}
		if (pacman->direction == DOWN && map->map[(int)round(pacmanX)][(int)round(pacmanY) - 1] != 1) {
			pacman->nextDirection = DOWN;
			pacman->direction = 0;
		}
		if (pacman->direction == LEFT && map->map[(int)round(pacmanX) - 1][(int)round(pacmanY)] != 1) {
			pacman->nextDirection = LEFT;
			pacman->direction = 0;
		}
		if (pacman->direction == RIGHT && map->map[(int)round(pacmanX) + 1][(int)round(pacmanY)] != 1) {
			pacman->nextDirection = RIGHT;
			pacman->direction = 0;
		}
	}

	// so for calculations, we adjust the coordinates to the middle of the pacman
	float pX = pacmanX + 0.5, pY = pacmanY + 0.5;

	// we find the local neighbourhood to check for collisions
	int leftTile = round(pX - 1), rightTile = round(pX + 1);
	int topTile = round(pY - 1), bottomTile = round(pY + 1);

	// now check collision
	bool xOverlaps = false, yOverlaps = false;
	bool collision = false;
	for (int i = leftTile; i <= rightTile; i++) {
		for (int j = topTile; j <= bottomTile; j++) {
			int tile = (*map).map[i][j];
			// collision with wall
			if (tile == 1) {
				xOverlaps = (pX - 0.5 < i + 1) && ((pX + 0.5) > i);
				yOverlaps = (pY - 0.5 < j + 1) && ((pY + 0.5) > j);

				collision = xOverlaps && yOverlaps;
				if (collision)
					(*pacman).position = vec2(round(pacmanX), round(pacmanY));
			}
			// collision with coins
			else if (tile == 2) {
				xOverlaps = (pX - 0.5 < i + 0.2) && ((pX + 0.5) > i + 0.8);
				yOverlaps = (pY - 0.5 < j + 0.2) && ((pY + 0.5) > j + 0.8);

				collision = xOverlaps && yOverlaps;
				if (collision) {
					(*map).map[i][j] = 0;
					this->coins--;
					//cout << coins << endl;
					if (this->coins == 0) {
						this->winConTriggered = true;
						(*map).map[0][17] = 0;
						(*map).map[34][17] = 0;
					}
				}
			}
			// collision with powerups 
			else if (tile == 3) {
				xOverlaps = (pX - 0.5 < i + 0.2) && ((pX + 0.5) > i + 0.8);
				yOverlaps = (pY - 0.5 < j + 0.2) && ((pY + 0.5) > j + 0.8);

				collision = xOverlaps && yOverlaps;
				if (collision) {
					(*map).map[i][j] = 0;
					(*pacman).powerUp = true;
					(*pacman).powerUpDuration = 10;
					(*pacman).currentSpeed = 0.15;
					// set ghosts to be frightened
					for (int l = 0; l < ghosts->size(); l++) {
						ghosts->at(l).color = vec3(1, 1, 1);
					}
				}
			}
		}
	}


	// pacman with exit
	if (winConTriggered) {
		// left exit
		xOverlaps = (pX - 0.5 < 0 + 1) && ((pX + 0.5) > 0);
		yOverlaps = (pY - 0.5 < 17 + 1) && ((pY + 0.5) > 17);

		collision = xOverlaps && yOverlaps;
		if (collision) {
			pacman->reset();
			for (int l = 0; l < ghosts->size(); l++) {
				ghosts->at(l).reset();
				ghosts->at(l).direction = 0;
			}
			wonGame();
		}

		// right exit 
		xOverlaps = (pX - 0.5 < 34 + 1) && ((pX + 0.5) > 34);
		yOverlaps = (pY - 0.5 < 17 + 1) && ((pY + 0.5) > 17);

		collision = xOverlaps && yOverlaps;
		if (collision) {
			pacman->reset();
			for (int l = 0; l < ghosts->size(); l++) {
				ghosts->at(l).reset();
				ghosts->at(l).direction = 0;
			}
			wonGame();
		}
	}

	// ghosts with walls and pacman
	for (int k = 0; k < ghosts->size(); k++) {
		float ghostX = ghosts->at(k).position.x, ghostY = ghosts->at(k).position.y;

		// so for calculations, we adjust the coordinates to the middle of the pacman
		float gX = ghostX + 0.5, gY = ghostY + 0.5;

		// we find the local neighbourhood to check for collisions
		leftTile = round(gX - 1), rightTile = round(gX + 1);
		topTile = round(gY - 1), bottomTile = round(gY + 1);

		// collision ghosts with pacman
		xOverlaps = false, yOverlaps = false;
		collision = false;
		xOverlaps = (gX - 0.5 < pX + 0.5) && ((gX + 0.5) > pX - 0.5);
		yOverlaps = (gY - 0.5 < pY + 0.5) && ((gY + 0.5) > pY - 0.5);

		collision = xOverlaps && yOverlaps;
		if (collision) {
			if ((*pacman).powerUp == true) {
				ghosts->at(k).reset();
			}
			else {
				for (int l = 0; l < ghosts->size(); l++) {
					ghosts->at(l).reset();
				}
				pacman->reset();
				pacman->life--;
				if (pacman->life == 0)
					gameOver();
				return;
			}
		}


		// collision ghosts with walls
		xOverlaps = false, yOverlaps = false;
		collision = false;
		for (int i = leftTile; i <= rightTile; i++) {
			for (int j = topTile; j <= bottomTile; j++) {
				int tile = (*map).map[i][j];
				// collision with wall
				// need to change collision with wall to be collision with intersections 
				// --- create a list of intersections and check collision with that
				if (tile == 1) {
					xOverlaps = (gX - 0.5 < i + 1) && ((gX + 0.5) > i);
					yOverlaps = (gY - 0.5 < j + 1) && ((gY + 0.5) > j);

					collision = xOverlaps && yOverlaps;
					if (collision) {
						ghosts->at(k).decideDirection();
						ghosts->at(k).position = vec2(round(ghostX), round(ghostY));
						break;
					}
				}
			}
		}

		
		// work in progress
		// collision ghosts with decision tiles
		// we don't want to do this if it already changed directions because of wall collision
		if (!collision) {
			pair<float, float> ghostPos(ghostX + 1, ghostY);
			if (isMidTile(ghostPos)) {
				pair<int, int> ghostPosInt = make_pair(ghostPos.first, ghostPos.second);
				if (find((*map).decisionTiles.begin(), (*map).decisionTiles.end(), ghostPosInt) != (*map).decisionTiles.end()) {
					ghosts->at(k).decideDirection();
				}
			}
		}
		
	}

}

bool Game::isMidTile(pair<float, float> tile) {
	if (fabsf(roundf(tile.first) - tile.first) <= 0.00001f && fabsf(roundf(tile.second) - tile.second) <= 0.00001f)
		return true;
	return false;
}
void Game::gameOver()
{
	lost = true;
	paused = true;
}

void Game::wonGame()
{
	won = true;
	paused = true;
}

int Game::countCoins()
{
	int count = 0;
	for (int i = 0; i < (*map).mapWidth; i++)
		for (int j = 0; j < (*map).mapHeight; j++)
			if ((*map).map[i][j] == 2)
				count++;
	//cout << count << endl << endl << endl;
	return count;
}

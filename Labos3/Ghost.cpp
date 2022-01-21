#include "Ghost.h"

Ghost::Ghost(GameMap* map, vec2 position, vec3 color)
{
	this->originalColor = color;
	this->color = color;
	this->originalPosition = position;
	this->position = position;
	this->map = map;
	this->position = position;
	this->currentSpeed = 0.1;
}

void Ghost::decideDirection()
{
	// checks which direction doesn't have a wall
	// and then picks a random direction from those
	// and sets it as a new direction
	vector<int> possibleDirections;

	int gX = round(position.x), gY = round(position.y);
	if ((*map).map[gX - 1][gY] != 1) possibleDirections.push_back(LEFT);
	if ((*map).map[gX + 1][gY] != 1) possibleDirections.push_back(RIGHT);
	if ((*map).map[gX][gY - 1] != 1) possibleDirections.push_back(DOWN);
	if ((*map).map[gX][gY + 1] != 1) possibleDirections.push_back(UP);

	direction = possibleDirections.at(rand() % possibleDirections.size());
}

void Ghost::updatePosition()
{
	// updating position depending on ghosts
	if (direction== UP) position.y += currentSpeed;
	if (direction == DOWN) position.y -= currentSpeed;

	if (direction == LEFT) position.x -= currentSpeed;
	if (direction == RIGHT) position.x += currentSpeed;
}

void Ghost::reset()
{
	this->position = this->originalPosition;
	this->decideDirection();
}

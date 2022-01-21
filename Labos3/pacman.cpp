

#include "pacman.h"


Pacman::Pacman() {
	originalPosition = vec2(17, 5);
	position = originalPosition;
	direction = 0;
	nextDirection = 0;
	originalSpeed = 0.1;
	currentSpeed = originalSpeed;
	powerUp = false;
	powerUpDuration = 10;
	life = 3;
};

void Pacman::updatePosition() {
	// updating position based on speed
	if (nextDirection == UP) position.y += currentSpeed;
	if (nextDirection == DOWN) position.y -= currentSpeed;

	if (nextDirection == LEFT) position.x -= currentSpeed;
	if (nextDirection == RIGHT) position.x += currentSpeed;
};

void Pacman::reset()
{
	this->position = this->originalPosition;
	this->direction = 0;
	this->nextDirection = 0;
	this->currentSpeed = this->originalSpeed;
};
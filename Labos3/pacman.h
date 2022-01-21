#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace std;
using namespace glm;

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

class Pacman
{
private:
public:
	int direction;
	int nextDirection;
	vec2 originalPosition;
	vec2 position;
	float originalSpeed;
	float currentSpeed;
	bool powerUp;
	float powerUpDuration;
	int life;

	Pacman();
	void updatePosition();
	void reset();

};
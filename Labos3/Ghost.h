#pragma once

#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>


#include "map.h"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

using namespace std;
using namespace glm;

class Ghost
{
private:
public:
	vec2 originalPosition;
	vec2 position;
	vec3 originalColor;
	vec3 color;
	int direction;
	float originalSpeed;
	float currentSpeed;
	bool frightened;

	GameMap* map;

	Ghost(GameMap* map, vec2 position, vec3 color);
	void decideDirection();
	void updatePosition();
	void reset();
};


#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "map.h"
#include "pacman.h"
#include "Game.h"
#include "Ghost.h"


using namespace std;
using namespace glm;

GLuint window;
GLuint width = 500, height = 500;

// OpenGL functions definition
void myDisplay();
void myIdle();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void myInit();

// function definitions
void drawMap();
void drawPacman();
void drawGhosts();
void drawGUI();
void displayGameOver();
void displayCongratz();
void displayPowerUp();
void diplayPauseMenu();

// global variables
bool paused = true;
bool gameOver = false;

float currentTime = 0.f;
float previousTime = 0.f;
float animationSpeed = 20;

GameMap gameMap;
Pacman pacman;
vector<Ghost> ghosts;
Game game(&gameMap, &pacman, &ghosts);

bool pacmanMouthOpen = false;
int pacmanAnimationTimer= 50;

int main(int argc, char** argv) {
	// random will be used to decide new ghost direction
	srand(time(NULL));


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Pacman");


	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutKeyboardFunc(myKeyboard);

	myInit();
	glutMainLoop();

	return 0;
}

void myInit() {
	// initializing glut settings
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 350.0, 0.0, 350.0);
	glPointSize(16.8);


	// setting up map and counting coins
	gameMap.setStartMap();
	game.coins = game.countCoins();

	// creating ghosts
	Ghost g1(&gameMap, vec2(16, 21), vec3(1, 0, 0));
	g1.decideDirection();
	ghosts.push_back(g1);

	Ghost g2(&gameMap, vec2(15, 21), vec3(1, 0, 1));
	g2.decideDirection();
	ghosts.push_back(g2);

	Ghost g3(&gameMap, vec2(18, 21), vec3(0, 1, 1));
	g3.decideDirection();
	ghosts.push_back(g3);

	Ghost g4(&gameMap, vec2(19, 21), vec3(0, 1, 0));
	g4.decideDirection();
	ghosts.push_back(g4);
}

// simple display function which class
// separate functions to draw different stuff
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	drawMap();
	drawPacman();
	drawGhosts();
	drawGUI();


	glutSwapBuffers();
}



void drawPacman() {
	// for pacman we always first draw a yellow square
	glBegin(GL_POLYGON);
	glColor3f(1.0, 1.0, 0.0);
	glVertex2f(pacman.position.x * 10, pacman.position.y * 10);
	glVertex2f(pacman.position.x * 10 + 10, pacman.position.y * 10);
	glVertex2f(pacman.position.x * 10 + 10, pacman.position.y * 10 + 10);
	glVertex2f(pacman.position.x * 10, pacman.position.y * 10 + 10);
	glEnd();

	// then we check if the mouth is open
	// if it is, we start drawing from the middle of the square
	// and then check which side the pacman is going
	// and point the mouth in that direction
	if (pacmanMouthOpen) {
		glBegin(GL_POLYGON);
		glColor3f(0.0, 0.0, 0.0);
		glVertex2f(pacman.position.x * 10 + 5, pacman.position.y * 10 + 5);
		if (pacman.nextDirection == DOWN) {
			glVertex2f(pacman.position.x * 10, pacman.position.y * 10);
			glVertex2f(pacman.position.x * 10 + 10, pacman.position.y * 10);
		}
		else if (pacman.nextDirection == RIGHT) {
			glVertex2f(pacman.position.x * 10 + 10, pacman.position.y * 10);
			glVertex2f(pacman.position.x * 10 + 10, pacman.position.y * 10 + 10);
		}
		else if (pacman.nextDirection == UP) {
			glVertex2f(pacman.position.x * 10 + 10, pacman.position.y * 10 + 10);
			glVertex2f(pacman.position.x * 10, pacman.position.y * 10 + 10);
		}
		else if (pacman.nextDirection == LEFT) {
			glVertex2f(pacman.position.x * 10, pacman.position.y * 10 + 10);
			glVertex2f(pacman.position.x * 10, pacman.position.y * 10);
		}
		glEnd();
	}
}

void drawGhosts() {
	// ghosts are draw as coloured squares
	for (int i = 0; i < ghosts.size(); i++) {
		glBegin(GL_POLYGON);
		glColor3f(ghosts.at(i).color.x, ghosts.at(i).color.y, ghosts.at(i).color.z);
		glVertex2f(ghosts.at(i).position.x * 10, ghosts.at(i).position.y * 10);
		glVertex2f(ghosts.at(i).position.x * 10 + 10, ghosts.at(i).position.y * 10);
		glVertex2f(ghosts.at(i).position.x * 10 + 10, ghosts.at(i).position.y * 10 + 10);
		glVertex2f(ghosts.at(i).position.x * 10, ghosts.at(i).position.y * 10 + 10);
		glEnd();
	}
}

// function responsible for drawing all GUI
// check variables to determine which state the game is in
// and displays appropriate messages
void drawGUI() {
	// display number of coins left to pick up
	glColor3f(1, 0, 0);
	glRasterPos2f(38, 26);
	string coins = "Coins left: " + to_string(game.countCoins());
	for (int i = 0; i < coins.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, coins[i]);
	}

	// display number of lives remaining
	glRasterPos2f(238, 26);
	string life = "Lives left: " + to_string(pacman.life);
	for (int i = 0; i < life.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, life[i]);
	}

	if (game.lost) {
		displayGameOver();
	}
	if (game.won) {
		displayCongratz();
	}
	if (!paused && pacman.powerUp) {
		displayPowerUp();
	}
	if (paused && !game.lost && !game.won) {
		diplayPauseMenu();
	}
}

// display pause message in the middle of the screen
void diplayPauseMenu() {
	glRasterPos2f(122, 115);
	string gameOver = "Press P to play!";
	for (int i = 0; i < gameOver.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameOver[i]);
	}
}

// display game over message in the middle of the screen
void displayGameOver() {
	glRasterPos2f(125, 115);
	string gameOver = "GAME OVER";
	for (int i = 0; i < gameOver.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameOver[i]);
	}
}

// display win message in the middle of the screen
void displayCongratz() {
	glRasterPos2f(138, 115);
	string gameWon = "YOU WIN";
	for (int i = 0; i < gameWon.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameWon[i]);
	}
}

// display power up remaining time message in the middle of the screen
void displayPowerUp() {
	glRasterPos2f(138, 115);

	stringstream stream;
	stream << std::fixed << std::setprecision(1) << pacman.powerUpDuration;

	string powerUp = "Time left: " + stream.str();
	for (int i = 0; i < powerUp.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, powerUp[i]);
	}
}

// map is a tile grid consisting of 0s, 1s, 2s and 3s
// 0 = nothing is there
// 1 = wall
// 2 = coins
// 3 = powerups
void drawMap() {
	// draw walls
	for (int i = 0; i < gameMap.mapWidth; i++) {
		for (int j = 0; j < gameMap.mapHeight; j++) {
			if (gameMap.map[i][j] == 1) {
				glBegin(GL_POLYGON);
				glColor3f(0.0, 0.0, 1.0);
				glVertex2f(i * 10, j * 10);
				glVertex2f(i * 10 + 10, j * 10);
				glVertex2f(i * 10 + 10, j * 10 + 10);
				glVertex2f(i * 10, j * 10 + 10);
				glEnd();
			}
		}
	}

	// draw coins
	for (int i = 0; i < gameMap.mapWidth; i++) {
		for (int j = 0; j < gameMap.mapHeight; j++) {
			if (gameMap.map[i][j] == 2) {
				glBegin(GL_POLYGON);
				glColor3f(0.0, 1.0, 0.0);
				glVertex2f(i * 10 + 5.5,j * 10 + 3);
				glVertex2f(i * 10 + 3,j * 10 + 5.5);
				glVertex2f(i * 10 + 5.5,j * 10 + 8);
				glVertex2f(i * 10 + 8,j * 10 + 5.5);
				glEnd();
			}
		}
	}

	// draw powers
	for (int i = 0; i < gameMap.mapWidth; i++) {
		for (int j = 0; j < gameMap.mapHeight; j++) {
			if (gameMap.map[i][j] == 3) {
				glBegin(GL_POLYGON);
				glColor3f(1.0, 0.0, 0.0);
				glVertex2f(i * 10 + 3, j * 10 + 3);
				glVertex2f(i * 10 + 8, j * 10 + 3);
				glVertex2f(i * 10 + 8, j * 10 + 8);
				glVertex2f(i * 10 + 3, j * 10 + 8);
				glEnd();
			}
		}
	}
}

void myIdle() {
	// update locations
	// check for collisions

	if (!paused) {
		currentTime = glutGet(GLUT_ELAPSED_TIME);
		int timeInterval = currentTime - previousTime;

		if (timeInterval > animationSpeed) {
			pacmanAnimationTimer--;
			// changes pacman mouth from open to close or vice versa every 10 ticks
			if (pacmanAnimationTimer <= 0) {
				pacmanAnimationTimer = 10;
				pacmanMouthOpen = !pacmanMouthOpen;
			}

			// update locations
			game.nextStep(timeInterval);

			// check for collisions
			game.checkCollision();
			
			// pause the game if user pressed p
			if (game.paused == true)
				paused = true;

			// display everything
			myDisplay();
			previousTime = currentTime;
		}
	}
	else {
		myDisplay();
		chrono::milliseconds duration(500);
		this_thread::sleep_for(duration);
	}

}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
	// get movement keys
	// check if pausing game
	if (theKey == 'p') {
		paused = !paused;
		if (paused)
			cout << "The game is now paused\n";
		else
			cout << "The game is now unpaused\n";
	}


	// checks if the direction of the pressed movement key is free to move into
	// if it is, we immediately change direction
	// if it isn't, we save the direction and then change it automatically later
	int pX = round(pacman.position.x);
	int pY = round(pacman.position.y);
	if (theKey == 'w')
		if (gameMap.map[pX][pY + 1] != 1)
			pacman.nextDirection = UP;
		else
			pacman.direction = UP;
	
	if (theKey == 's') 
		if (gameMap.map[pX][pY - 1] != 1)
			pacman.nextDirection = DOWN;
		else
			pacman.direction = DOWN;

	if (theKey == 'a')
		if (gameMap.map[pX - 1][pY] != 1)
			pacman.nextDirection = LEFT;
		else
			pacman.direction = LEFT;
	if (theKey == 'd')
		if (gameMap.map[pX + 1][pY] != 1)
			pacman.nextDirection = RIGHT;
		else
			pacman.direction = RIGHT;
}


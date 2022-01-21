#pragma once

//  #include <GL/Gl.h>
//  #include <GL/Glu.h>    nije potrebno ako se koristi glut
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include <string>
#include <fstream>
#include <vector>
#include <iostream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4


using namespace std;
using namespace glm;


class Vertex {
public:
	float x, y, z;
	Vertex(float x1, float y1, float z1) {
		x = x1; y = y1; z = z1;
	}
};

class Particle {
public:
	float x, y, z;
	float osX, osY, osZ;
	float sX, sY, sZ;
	int age = 0;
	double size;
	float colorR, colorG, colorB;
	int lifespan;
	double angle;
};
class Source {
public:
	float x, y, z;
	int gen;
	double size = 256;
	float colorR, colorG, colorB;
};

GLuint window;
GLuint width = 512, height = 512;

int currentTime = 0, previousTime = 0;

vector<Particle> particles;
Source source;


Vertex vertexIshodiste(0, 0, 0);
Vertex ociste(0.0, 0.0, 76.0);
GLuint texture;


GLuint loadTexture();
void drawP(Particle p);
void drawParticles();
void myDisplay();
void myIdle();

void myKeyboard(unsigned char theKey, int mouseX, int mouseY);


int main(int argc, char** argv) {
	srand(time(NULL));


	source.x = 0.f;	source.y = 1.f;	source.z = 76.5f;
	source.gen = 4;
	source.colorR = 1.0;	source.colorG = 1.0;	source.colorB = 1.0;
	source.size = 0.05;



	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.5f, 3000.0f);

	window = glutCreateWindow("Animacija");


	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);

	glutKeyboardFunc(myKeyboard);

	texture = loadTexture();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glutMainLoop();

	return 0;
}

GLuint loadTexture() {
	GLuint texture;
	const char* texturePath = "textures/snow.bmp";

	int twidth = 256, theight = 256;

	BYTE* data;
	FILE* file;

	file = fopen(texturePath, "rb");
	data = (BYTE*)malloc(twidth * theight * 3);
	fread(data, twidth * theight * 3, 1, file);
	fclose(file);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, twidth, theight, GL_RGB, GL_UNSIGNED_BYTE, data);

	free(data);

	return texture;
}


void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(ociste.x, ociste.y, -ociste.z);

	drawParticles();

	//cout << "source(" << source.x << ", " << source.y<< ", " << source.z << ")\n";
	//cout << source.gen << endl;
	glutSwapBuffers();
}
vec3 generateParticleSpeed() {
	double x, y, z, norm;
	x = ((double)(rand() % 2000) - 1000) / 1000;
	y = - ((double)(rand() % 1000)) / 1000;
	z = ((double)(rand() % 2000) - 1000) / 1000;

	vec3 speed(x, y, z);

	return normalize(speed);
}
void myIdle() {
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 50) {
		if (source.gen > 0) {
			int n = rand() % source.gen + 1;
			for (int i = 0; i < n; i++) {
				vec3 speed = generateParticleSpeed();

				Particle p;

				p.x = source.x;	p.y = source.y;	p.z = source.z;
				p.colorR = source.colorR;	p.colorB = source.colorB;	p.colorG = source.colorG;
				p.sX = speed.x;	p.sY = speed.y;	p.sZ = speed.z;
				p.size = source.size;
				p.age = 0;
				p.lifespan = 100 + (rand() % 20 - 10);
				particles.push_back(p);
			}
		}
		cout << "Trenutno zivi " << particles.size() << " cestica.\n";


		for (int i = 0; i < particles.size(); i++) {
			// update particle age and delete it if it's too old

			particles.at(i).age += 1;
			if (particles.at(i).age > particles.at(i).lifespan) {
				particles.erase(particles.begin() + i);
				i--;
				continue;
			}

			Vertex s(0, 0, 1);
			Vertex e(0, 0, 0);
			Vertex os(0, 0, 0);


			e.x = particles.at(i).x - ociste.x;
			e.y = particles.at(i).y - ociste.x;
			e.z = particles.at(i).z - ociste.x;

			os.x = 1.f * s.y * e.z - 1.f * e.y * s.z;
			os.y = -(1.f * s.x * e.z - 1.f * e.x * s.z);
			os.z = 1.f * s.x * e.y - 1.f * s.y * e.x;

			double duljinaS = sqrt(pow((float)s.x, 2.0) + pow((float)s.y, 2.0) + pow((float)s.z, 2.0));
			double duljinaE = sqrt(pow((float)e.x, 2.0) + pow((float)e.y, 2.0) + pow((float)e.z, 2.0));
			double kut = acos((s.x * e.x + s.y * e.y + s.z * e.z) / (duljinaS * duljinaE));
			kut = (kut / 3.14) * 180;


			particles.at(i).angle = kut;
			particles.at(i).osX = os.x;	particles.at(i).osY = os.y;	particles.at(i).osZ = os.z;



			particles.at(i).x += 0.01 * particles.at(i).sX;
			particles.at(i).y += 0.01 * particles.at(i).sY;
			particles.at(i).z += 0.01 * particles.at(i).sZ;

			particles.at(i).colorR = (particles.at(i).colorR - 0.01 > 0) ? particles.at(i).colorR - 0.01 : 0;
			particles.at(i).colorG = (particles.at(i).colorG - 0.01 > 0) ? particles.at(i).colorG - 0.01 : 0;
		}

		source.gen += 1;
		if (source.gen == 15)
			source.gen = -45;

		myDisplay();
		previousTime = currentTime;
	}
}

void drawP(Particle p) {

	glColor3f(p.colorR, p.colorG, p.colorB);
	glTranslatef(p.x, p.y, p.z);
	glRotatef(p.angle, p.osX, p.osY, p.osZ);
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0); glVertex3f(-p.size, -p.size, 0.0);
	glTexCoord2d(1.0, 0.0); glVertex3f(p.size, -p.size, 0.0);
	glTexCoord2d(1.0, 1.0); glVertex3f(p.size, p.size, 0.0);
	glTexCoord2d(0.0, 1.0); glVertex3f(-p.size, p.size, 0.0);

	glEnd();
	glRotatef(-p.angle, p.osX, p.osY, p.osZ);
	glTranslatef(-p.x, -p.y, -p.z);
	//cout << "Particle position:\n" << "\tx = " << p.x << "\ty = " << p.y << "\tz = " << p.z << "\n\n";
	//cout << p.size << "\n";
}

void drawParticles() {
	for (int i = 0; i < particles.size(); i++) {
		drawP(particles.at(i));
	}
}


void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
	if (theKey == 'a') source.x -= 0.1;
	if (theKey == 'd') source.x += 0.1;
	if (theKey == 'w') source.z -= 0.1;
	if (theKey == 's') source.z += 0.1;
	if (theKey == 'o') source.y -= 0.1;
	if (theKey == 'p') source.y += 0.1;
}
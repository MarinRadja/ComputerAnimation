#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>


#include <GL/glut.h>

using namespace std;
using namespace glm;


int brojVrhova = 0, brojTrokuta = 0;
int brojB = 0, brojSegmenata = 0;

float currentTime = 0.f;
float previousTime = 0.f;
float animationSpeed = 200;
int iVrh = 0;
float tSeg = 0.f;


class Vrh {
public:
	float x, y, z;
	Vrh(float x1, float y1, float z1) {
		x = x1; y = y1; z = z1;
	}
};
class Trokut {
public:
	Vrh* v1, * v2, * v3;
	Trokut(Vrh* vX, Vrh* vY, Vrh* vZ) {
		v1 = vX; v2 = vY; v3 = vZ;
	}
};

Vrh* vrhovi;
Trokut* trokuti;
Vrh* vrhB;
Vrh* putanjaB;
Vrh* putanjaDB, * sveTanB;

Vrh srediste(0, 0, 0);
Vrh sredisteTrokuta(0, 0, 0);
Vrh os(0, 0, 0);
Vrh s(0, 0, 1);
Vrh e(0, 0, 0);


// matrice
mat4 B = glm::mat4(
	vec4(-1, 3, -3, 1),
	vec4(3, -6, 0, 4),
	vec4(-3, 3, 3, 1),
	vec4(1, 0, 0, 0)
);
mat4x3 dB = glm::mat4x3(
	vec3(-1, 2, -1),
	vec3(3, -4, 0),
	vec3(-3, 2, 1),
	vec3(1, 0, 0)
);
mat4x2 ddB = glm::mat4x2(
	vec2(-1, 1),
	vec2(3, -2),
	vec2(-3, 1),
	vec2(1, 0)
);

// GLut window
GLuint window;
GLuint width = 700, height = 500;
GLuint sub_height, sub_width;


void loadObject(string file);
void loadBSpline(string file);
void calculatePath();

bool startsWith(string text, string prefix);


void myDisplay();
void myIdle();
void drawTrajectory();

int main(int argc, char** argv) {
	// load object
	loadObject(argv[1]);

	// load spline
	loadBSpline(argv[2]);

	calculatePath();


	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Animacija");

	gluLookAt(0, 0.5, 1, 0, 20, 50, 1, 0, 0);

	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}

void loadObject(string file) {
	ifstream infile(file);

	vector<string> objectText;

	string line;
	while (getline(infile, line)) {
		if (startsWith(line, "v")) brojVrhova++;
		else if (startsWith(line, "f")) brojTrokuta++;
		objectText.push_back(line);
	}

	vrhovi = (Vrh*)malloc(sizeof(Vrh) * brojVrhova);
	trokuti = (Trokut*)malloc(sizeof(Trokut) * brojTrokuta);

	int iVrh = 0;
	for (int i = 0; i < objectText.size(); i++) {

		stringstream ss;
		char c;
		float f1, f2, f3;

		if (startsWith(objectText[i], "v")) {
			ss.clear();

			ss << objectText[i];
			ss >> c >> f1 >> f2 >> f3;

			Vrh v(f1, f2, f3);
			srediste.x += v.x;
			srediste.y += v.y;
			srediste.z += v.z;
			vrhovi[iVrh] = v;
			iVrh++;
		}
		ss.clear();
	}
	srediste.x /= iVrh;
	srediste.y /= iVrh;
	srediste.z /= iVrh;

	iVrh = 0;

	int iTrokut = 0;
	for (int i = 0; i < objectText.size(); i++) {
		stringstream ss;
		char c;
		int i1, i2, i3;

		if (startsWith(objectText[i], "f")) {
			ss << objectText[i];
			ss >> c >> i1 >> i2 >> i3;


			Trokut t(&vrhovi[i1 - 1], &vrhovi[i2 - 1], &vrhovi[i3 - 1]);
			trokuti[iTrokut] = t;
			iTrokut++;

			Vrh v1 = vrhovi[i1 - 1];
			Vrh v2 = vrhovi[i2 - 1];
			Vrh v3 = vrhovi[i3 - 1];
			sredisteTrokuta.x += v1.x + v2.x + v3.x;
			sredisteTrokuta.y += v1.y + v2.y + v3.y;
			sredisteTrokuta.z += v1.z + v2.z + v3.z;
		}
	}

	sredisteTrokuta.x /= brojTrokuta;
	sredisteTrokuta.y /= brojTrokuta;
	sredisteTrokuta.z /= brojTrokuta;

	objectText.clear();
}

void loadBSpline(string file) {
	ifstream infile(file);

	vector<string> splineText;

	string line;
	while (getline(infile, line)) {
		splineText.push_back(line);
	}

	brojB = splineText.size();
	vrhB = (Vrh*)malloc(sizeof(Vrh) * brojB);
	brojSegmenata = brojB - 3;

	stringstream ss;
	float f1, f2, f3;
	for (int i = 0; i < brojB; i++) {
		ss.clear();

		ss << splineText[i];
		ss >> f1 >> f2 >> f3;

		Vrh v1(f1, f2, f3);
		vrhB[i] = v1;
	}
}

Vrh calcPathVertex(float t, int i) {
	// p(t) = T * 1/6 * B * R

	vec4 tVec = vec4(pow(t, 3), pow(t, 2), t, 1);

	mat3x4 rMat = mat3x4(
		vec4(vrhB[i].x, vrhB[i + 1].x, vrhB[i + 2].x, vrhB[i + 3].x),
		vec4(vrhB[i].y, vrhB[i + 1].y, vrhB[i + 2].y, vrhB[i + 3].y),
		vec4(vrhB[i].z, vrhB[i + 1].z, vrhB[i + 2].z, vrhB[i + 3].z)
	);

	vec3 p = tVec * 0.16666f * B * rMat;

	return Vrh(p.x, p.y, p.z);
}

Vrh  calcDP(float t, int i) {
	// p'(t) = T * 1/2 * B' * R

	vec3 tVec = vec3(pow(t, 2), t, 1);

	mat3x4 rMat = mat3x4(
		vec4(vrhB[i].x, vrhB[i + 1].x, vrhB[i + 2].x, vrhB[i + 3].x),
		vec4(vrhB[i].y, vrhB[i + 1].y, vrhB[i + 2].y, vrhB[i + 3].y),
		vec4(vrhB[i].z, vrhB[i + 1].z, vrhB[i + 2].z, vrhB[i + 3].z)
	);

	vec3 dp = tVec * 0.5f * dB * rMat;

	return Vrh(dp.x, dp.y, dp.z);
}

Vrh  calcDDP(float t, int i) {
	// p''(t) = T * B'' * R

	vec2 tVec = vec2(t, 1);

	mat3x4 rMat = mat3x4(
		vec4(vrhB[i].x, vrhB[i + 1].x, vrhB[i + 2].x, vrhB[i + 3].x),
		vec4(vrhB[i].y, vrhB[i + 1].y, vrhB[i + 2].y, vrhB[i + 3].y),
		vec4(vrhB[i].z, vrhB[i + 1].z, vrhB[i + 2].z, vrhB[i + 3].z)
	);

	vec3 ddp = tVec * ddB * rMat;

	return Vrh(ddp.x, ddp.y, ddp.z);
}


void calculatePath() {
	putanjaB = (Vrh*)malloc(sizeof(Vrh) * brojSegmenata * 100);

	int iTan = 0;
	int iSvihTan = 0;

	for (int i = 0; i < brojSegmenata; i++) {
		for (int t = 0; t < 100; t++) {
			putanjaB[i * 100 + t] = calcPathVertex(1.f * t / 100, i);
		}
	}
}



// GLut fje

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);

	glBegin(GL_LINE_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < brojSegmenata * 100; i++) {
		Vrh v = putanjaB[i];

		glVertex3f(v.x / 25.f, v.y / 25.f, v.z / 25.f);
	}
	glEnd();

	// tocka na krivulji
	Vrh osStart = calcPathVertex(tSeg, iVrh);
	vec3 osStartV = vec3(osStart.x, osStart.y, osStart.z);


	// tangenta
	glColor3f(.0, 1., .0);
	Vrh tan = calcDP(tSeg, iVrh);
	vec3 tanV = normalize(vec3(tan.x, tan.y, tan.z));
	vec3 tanStartVec = vec3(osStart.x, osStart.y, osStart.z);
	vec3 tanEndVec = tanStartVec + 5.f * tanV;
	glBegin(GL_LINES);
	glVertex3f(tanStartVec.x / 25.f, tanStartVec.y / 25.f, tanStartVec.z / 25.f);
	glVertex3f(tanEndVec.x / 25.f, tanEndVec.y / 25.f, tanEndVec.z / 25.f);
	glEnd();

	// normala
	glColor3f(.0, .0, 1.0);
	Vrh ddp = calcDDP(tSeg, iVrh);
	vec3 normV = normalize(cross(tanV, vec3(ddp.x, ddp.y, ddp.z)));
	vec3 normStartVec = vec3(osStart.x, osStart.y, osStart.z);
	vec3 normEndVec = normStartVec + 5.f * normV;
	glBegin(GL_LINES);
	glVertex3f(normStartVec.x / 25.f, normStartVec.y / 25.f, normStartVec.z / 25.f);
	glVertex3f(normEndVec.x / 25.f, normEndVec.y / 25.f, normEndVec.z / 25.f);
	glEnd();

	// binormala
	glColor3f(1.0, .0, .0);
	vec3 biNormV = normalize(cross(tanV, normV));
	vec3 biNormStartVec = vec3(osStart.x, osStart.y, osStart.z);
	vec3 biNormEndVec = biNormStartVec + 5.f * biNormV;
	glBegin(GL_LINES);
	glVertex3f(biNormStartVec.x / 25.f, biNormStartVec.y / 25.f, biNormStartVec.z / 25.f);
	glVertex3f(biNormEndVec.x / 25.f, biNormEndVec.y / 25.f, biNormEndVec.z / 25.f);
	glEnd();


	mat3 rotationMat = mat3(normV, biNormV, tanV);

	vec3 center = vec3(sredisteTrokuta.x, sredisteTrokuta.y, sredisteTrokuta.z);

	// rotation with dcm
	if (true) {
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < brojTrokuta; i++) {
			Vrh v1 = *trokuti[i].v1;
			Vrh v2 = *trokuti[i].v2;
			Vrh v3 = *trokuti[i].v3;

			vec3 v1Vec = vec3(v1.x, v1.y, v1.z);
			vec3 v2Vec = vec3(v2.x, v2.y, v2.z);
			vec3 v3Vec = vec3(v3.x, v3.y, v3.z);

			vec3 v1Final = ((v1Vec - center) * inverse(rotationMat)) / 5.f + osStartV / 25.f;
			vec3 v2Final = ((v2Vec - center) * inverse(rotationMat)) / 5.f + osStartV / 25.f;
			vec3 v3Final = ((v3Vec - center) * inverse(rotationMat)) / 5.f + osStartV / 25.f;

			glVertex3f(v1Final.x, v1Final.y, v1Final.z);
			glVertex3f(v2Final.x, v2Final.y, v2Final.z);
			glVertex3f(v3Final.x, v3Final.y, v3Final.z);
		}
		glEnd();
		glPopMatrix();
	}



	if (false) {
		e.x = tan.x;
		e.y = tan.y;
		e.z = tan.z;

		os.x = 1.f * s.y * e.z - 1.f * e.y * s.z;
		os.y = -(1.f * s.x * e.z - 1.f * e.x * s.z);
		os.z = 1.f * s.x * e.y - 1.f * s.y * e.x;

		float duljinaS = sqrt(pow((float)s.x, 2.0) + pow((float)s.y, 2.0) + pow((float)s.z, 2.0));
		float duljinaE = sqrt(pow((float)e.x, 2.0) + pow((float)e.y, 2.0) + pow((float)e.z, 2.0));
		float kut = acos(s.x * e.x + s.y * e.y + s.z * e.z / (duljinaS * duljinaE));
		kut = (kut / 3.14) * 180;


		glPushMatrix();
		glTranslatef(osStartV.x / 25.f, osStartV.y / 25.f, osStartV.z / 25.f);
		glRotatef(kut, os.x, os.y, os.z);
		glTranslatef(-center.x / 5.f, -center.y / 5.f, -center.z / 5.f);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < brojTrokuta; i++) {
			Vrh v1 = *trokuti[i].v1;
			Vrh v2 = *trokuti[i].v2;
			Vrh v3 = *trokuti[i].v3;

			glVertex3f(v1.x / 5.f, v1.y / 5.f, v1.z / 5.f);
			glVertex3f(v2.x / 5.f, v2.y / 5.f, v2.z / 5.f);
			glVertex3f(v3.x / 5.f, v3.y / 5.f, v3.z / 5.f);
		}
		glEnd();
		glPopMatrix();
	}

	// move to next position
	tSeg += 0.1f;
	if (tSeg >= 1.f) {
		iVrh++;
		tSeg = 0.f;
	}
	if (iVrh > brojSegmenata-1) {
		iVrh = 0;
		tSeg = 0.f;
	}

	glFlush();
}

void myIdle() {
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 40) {
		myDisplay();
		previousTime = currentTime;
	}
}



bool startsWith(string text, string prefix) {
	if (text.rfind(prefix, 0) == 0)
		return true;
	return false;
}
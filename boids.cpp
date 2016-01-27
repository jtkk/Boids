// Jolie Kawasaki
// June 3, 2015
// boids.cpp

#include <glew.h>
#include <freeglut.h>
#include <time.h>
#include <vector>
#include "GLSL.h"
#include "Boid.h"

int window;
GLint winWidth = 800;
GLint winHeight = 600;
const float midWidth = winWidth / 2;
const float midHeight = winHeight / 2;

const int MAX_BOIDS = 70;
const int MAX_PREDATORS = 5;
const int MAX_TARGETS = 3;

const int LO = -1; // Position minimum
const int HI = 1; // Position maximum
const float VMIN = -.0002; // Velocity minimum
const float VMAX = .0002; // Velocity maximum
const float iVX = .00015; // Default initial x-velocity
const float iVY = .00015; // Default initial y-velocity
const int initBoids = 50; // Number of initial boids.

const float dotSize = 7.00f;

std::vector<Pair> targets;
std::vector<Boid> predators;
std::vector<Boid> flock;

float randPos() { // Sets random position.
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

float randVel() { // Returns random velocity.
	return VMIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (VMAX - VMIN)));
}

float normX(int x) {
	if (x < midWidth) {
		x = -(midWidth - x);
	}
	else if (x >= midWidth) {
		x = x - midWidth;
	}
	return (x / midWidth);
}

float normY(int y) {
	if (y >= midHeight) {
		y = -(y - midHeight);
	}
	else if (y < midHeight) {
		y = midHeight - y;
	}
	return (y / midHeight);
}

void addBoid(float posx, float posy) {
	Boid b(randVel(), randVel(), posx, posy);
	//Boid b(iVX, iVY, posx, posy);
	flock.push_back(b);
}

void addTarget(float posx, float posy) {
	Pair t(posx, posy);
	targets.push_back(t);
}

void addPredator(float posx, float posy) {
	Boid p(randVel(), randVel(), posx, posy);
	//Boid p(iVX, iVY, posx, posy);
	predators.push_back(p);
}

void initialize() {
	for (int i = 0; i < initBoids; i++) {
		addBoid(randPos(), randPos());
	}
}

// BOID RULES

// Avoid collisions with nearby flockmates.
// Boids try to keep a small distance away from other objects, including other boids.
Pair collisionRule(int b) {
	Pair d(0, 0);
	for (int i = 0; i < flock.size(); i++) {
		if (b != i) {
			if (d.magnitude(flock[i].position - flock[b].position) < 0.03) {
				d = d - (flock[i].position - flock[b].position);
			}
		}
	}
	return d;
}

// Attempt to match velocity with nearby flockmates.
Pair velocityRule(int b) {
	int count = 0;
	Pair v;
	for (int i = 0; i < flock.size(); i++) {
		if (b != i) {
			if (v.magnitude(flock[i].position - flock[b].position) < 0.4) {
				v = v + flock[i].velocity;
				count++;
			}
		}
	}
	if (count < 1)
		return v;
	v = v / count;
	return (v - flock[b].velocity) / 8;
}

// Attempt to stay close to nearby flockmates.
// Boids try to fly towards the center of mass of neighboring boids.
Pair centeringRule(int b) {
	int count = 0;
	Pair c;
	for (int i = 0; i < flock.size(); i++) {
		if (b != i) {
			if (c.magnitude(flock[i].position - flock[b].position) < 0.4) {
				c = c + flock[i].position;
				count++;
			}
		}
	}
	if (count < 1)
		return c;
	c = c / count;
	return (c - flock[b].position) / 100;
}

Pair seek(int b) {
	Pair temp;
	temp = targets[0];
	for (int i = 1; i < targets.size(); i++) { // How to deal with more than one food??????
		if (temp.magnitude(temp - flock[b].position)
			< temp.magnitude(targets[i] - flock[b].position)) {
			temp = targets[i];
		}
	}
	return (temp - flock[b].position) / 100;
}

Pair avoid(int b) {
	int count = 0;
	Pair temp = predators[0].position;
	for (int i = 1; i < predators.size(); i++) {
		if (temp.magnitude(temp - flock[b].position)
			> temp.magnitude(predators[i].position - flock[b].position)) {
			temp = predators[i].position;
		}
	}
	if (temp.magnitude(temp - flock[b].position) > 0.3) {
		return 0;
	}
	return (temp - flock[b].position) / 100;
}

void bounds(int i) {
	if (flock[i].position.x > 1) {
		int x = flock[i].position.x * winWidth;
		x = x + winWidth;
		flock[i].position.x = -flock[i].position.x;
	}
	else if (flock[i].position.x < -1) {
		flock[i].position.x = -flock[i].position.x;
	}
	if (flock[i].position.y > 1) {
		flock[i].velocity.y = -iVY;
	}
	else if (flock[i].position.y < -1) {
		flock[i].velocity.y = iVY;
	}
}

// Limit boid velocity
void vmax(int i) {
	Pair v;
	if (v.magnitude(flock[i].velocity.x) > VMAX) {
		flock[i].velocity.x = (flock[i].velocity.x / v.magnitude(flock[i].velocity.x) * VMAX);
	}
	if (v.magnitude(flock[i].velocity.y) > VMAX) {
		flock[i].velocity.y = (flock[i].velocity.y / v.magnitude(flock[i].velocity.y) * VMAX);
	}
}


float w1 = 1;
float w2 = 1;
float w3 = 1.5;
float w4 = 0.75;
float w5 = -1.5;

void moveBoids() {
	for (int i = 0; i < flock.size(); i++) {
		Pair r1, r2, r3, r4, r5 = 0;

		if (flock.size() > 1) {
			r1 = collisionRule(i) * w1;
			r2 = velocityRule(i) * w2;
			r3 = centeringRule(i) * w3;
		}
		if (!targets.empty()) {
			r4 = seek(i) * w4;
		}
		if (!predators.empty()) {
			r5 = avoid(i) * w5;
		}
		flock[i].velocity = flock[i].velocity + r1 + r2 + r3 + r4 + r5;
		vmax(i);
		flock[i].position = flock[i].position + flock[i].velocity;
		bounds(i);
	}
}


Pair pcollision(int p) {
	Pair d(0, 0);
	for (int i = 0; i < predators.size(); i++) {
		if (p != i) {
			if (d.magnitude(predators[i].position - predators[p].position) < 0.03) {
				d = d - (predators[i].position - predators[p].position);
			}
		}
	}
	return d;
}

Pair pseek(int p) {
	Pair temp;
	temp = flock[0].position;
	for (int i = 0; i < (flock.size() - 1); i++) {
		if (temp.magnitude(flock[i].position - predators[p].position)
			< temp.magnitude(flock[i + 1].position - predators[p].position)) {
			temp = flock[i].position;
		}
	}
	return (temp - predators[p].position) / 100;
}


void pbounds(int i) {
	if (predators[i].position.x > 1) {
		int x = predators[i].position.x * winWidth;
		x = x + winWidth;
		predators[i].position.x = -predators[i].position.x;
	}
	else if (predators[i].position.x < -1) {
		predators[i].position.x = -predators[i].position.x;
	}
	if (predators[i].position.y > 1) {
		predators[i].velocity.y = -iVY;
	}
	else if (predators[i].position.y < -1) {
		predators[i].velocity.y = iVY;
	}
}

// Limit boid velocity
void pvmax(int i) {
	Pair v;
	if (v.magnitude(predators[i].velocity.x) > VMAX) {
		predators[i].velocity.x = (predators[i].velocity.x / v.magnitude(predators[i].velocity.x) * VMAX);
	}
	if (v.magnitude(predators[i].velocity.y) > VMAX) {
		predators[i].velocity.y = (predators[i].velocity.y / v.magnitude(predators[i].velocity.y) * VMAX);
	} 
}


float p1 = 1;
float p2 = 1.5;

void movePredators() {
	for (int i = 0; i < predators.size(); i++) {
		Pair r1, r2;
		r1 = pcollision(i) * p1;
		if (flock.size() > 0) {
			r2 = pseek(i) * p2;
		}
		predators[i].velocity = predators[i].velocity + r1 + r2;
		pvmax(i);
		predators[i].position = predators[i].position + predators[i].velocity;
		pbounds(i);
	}
}

// Draw each individual Boid
void drawBoids() {
	for (int i = 0; i < flock.size(); i++) {
		glPointSize(dotSize);
		glBegin(GL_POINTS);
		glColor3f(flock[i].red, flock[i].green, flock[i].blue);
		glVertex2f(flock[i].position.x, flock[i].position.y);
		glEnd();
	}
}

void drawTargets() {
	for (int i = 0; i < targets.size(); i++) {
		glPointSize(dotSize);
		glBegin(GL_POINTS);
		glColor3f(1, 0, 0);
		glVertex2f(targets[i].x, targets[i].y);
		glEnd();
	}
}

void drawPredators() {
	for (int i = 0; i < predators.size(); i++) {
		glPointSize(dotSize);
		glBegin(GL_POINTS);
		glColor3f(0, 1, 0);
		glVertex2f(predators[i].position.x, predators[i].position.y);
		glEnd();
	}
}


void Display() {

	// background, blending, zbuffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	drawBoids();
	drawTargets();
	drawPredators();
	moveBoids();
	movePredators();

	glFlush();
}


// Interaction
void MouseAdd(int butn, int state, int x, int y) { // Add a boid at the position of the cursor.
	if (state == GLUT_LEFT_BUTTON) {
		if (flock.size() < MAX_BOIDS) {
			float px, py = 0;
			px = normX(x);
			py = normY(y);
			addBoid(px, py);
		}
	}
	glutPostRedisplay();
}


bool pause = false;
void Key(unsigned char key, int x, int y) {
	if (key == 27) {
		glutDestroyWindow(window);
		exit(0);
	}
	if (key == 127) {
		if (flock.size() > 0) {
			flock.pop_back();
		}
	}
	if (key == 's') { // scatter flock by negating rule 
		w3 = -w3;
	}
	if (key == 'f') {
		if (targets.size() < MAX_TARGETS) {
			addTarget(randPos(), randPos());
		}
	}
	if (key == 'd') {
		if (targets.size() > 0) {
			targets.pop_back();
			if (targets.size() < 1) {
				w4 = -1;
			}
		}
	}
	if (key == 'p') {
		if (predators.size() < MAX_PREDATORS) {
			addPredator(randPos(), randPos());
		}
	}
	if (key == 'o') {
		if (predators.size() > 0) {
			predators.pop_back();
		}
	}
	if (key == 32) {
		pause = !pause;
	}
}

void Idle() {
	if (!pause) {
		glutPostRedisplay();
	}
}

//void Close()
//{
//	// unbind vertex buffer, free GPU memory
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glDeleteBuffers(1, &vBufferID);
//}

void main(int ac, char **av) {
	srand(time(NULL));

	// init app window
	glutInit(&ac, av);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	window = glutCreateWindow("2D Boids");
	GLenum err = glewInit();
	if (err != GLEW_OK)
		printf("Error initializaing GLEW: %s\n", glewGetErrorString(err));

	initialize();

	// callbacks
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(MouseAdd);
	glutKeyboardFunc(Key);
	glutMainLoop();
}
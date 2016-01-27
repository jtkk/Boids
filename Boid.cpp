// Jolie Kawasaki
// April 30, 2015
// Boid.cpp

#include "Boid.h"

Boid::Boid(float velX, float velY, float posX, float posY) {
	setBoid(velX, velY, posX, posY);
}

void Boid::setBoid(float velX, float velY, float posX, float posY) {
	position.x = posX;
	position.y = posY;
	velocity.x = velX;
	velocity.y = velY;
}
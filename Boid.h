// Jolie Kawasaki
// April 30, 2015
// Boid.h

#ifndef BOID_H
#define BOID_H

#include "Pair.h"

class Boid {
public:

	Pair position;
	Pair velocity;
	float red = 0;
	float green = 0.5;
	float blue = 1;
    
	Boid() {
		setBoid(0, 0, 0, 0);
	}
	Boid(float posX, float posY, float velX, float velY);
	~Boid(){}

	// Set methods
	void setBoid(float velX, float velY, float posX, float posY);

	void setPXX(float x) { position.x = x; }
	void setPY(float y) { position.y = y; }
	void setVX(float x) { velocity.x = x; }
	void setVY(float y) { velocity.y = y; }

};

#endif

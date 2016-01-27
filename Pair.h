// Jolie Kawasaki
// May 21, 2015
// Pair.h
// A pair struct for use in Boids project.

#ifndef PAIR_H
#define PAIR_H

#include <math.h>

struct Pair{
	float x;
	float y;

	Pair(float x = 0, float y = 0) :
		x(x), y(y) {}

	Pair operator+(const float a) const {
		return Pair(x + a, y + a);
	}

	Pair operator-(const float a) const {
		return Pair(x - a, y - a);
	}

	Pair operator*(const float a) const {
		return Pair(x * a, y * a);
	}

	Pair operator/(const float a) const {
		return Pair(x / a, y / a);
	}

	Pair operator+(const Pair& a) const {
		return Pair(x + a.x, y + a.y);
	}

	Pair operator-(const Pair& a) const {
		//Pair temp(a.x * -1, a.y * -1);
		//return *this + temp;
		return Pair(x - a.x, y - a.y);
	}

	Pair operator*(const Pair& a) const {
		return Pair(x * a.x, y * a.y);
	}

	Pair operator/(const Pair& a) const {
		return Pair(x / a.x, y / a.y);
	}

	bool operator>(const float a) const {
		return (x > a && y > a);
	}

	bool operator<(const float a) const {
		return (x < a && y < a);
	}

	Pair absolute(Pair var) {
		if (var.x < 0)
			var.x = -var.x;
		if (var.y < 0)
			var.y = -var.y;
		return var;
	}

	float magnitude(Pair var) {
		return sqrt((var.x * var.x) + (var.y * var.y));
	}
};

#endif
#include "Math.h"

const float MathAddon::PI = 3.14159265359f;

float MathAddon::angleRadToDeg(float angle) {
	return angle * 180.0f / PI;
}


float MathAddon::angleDegToRad(float angle) {
	return angle * PI / 180.0f;
}

//normalise un vecteur en mettant sa longueur � 1 tout en gardant sa direction
Vector2D Vector2D::normalize() {
	float magnitudeStored = magnitude();
	if (magnitudeStored > 0.0f) {
		//divise x et y par sa longueur pour d�finir sa longueur � 1
		//Car deux meme valeur qui se divise sont �gale � 1
		x /= magnitudeStored;
		y /= magnitudeStored;
	}

	return *this;
}
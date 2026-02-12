#include "raylib.h"

typedef struct {
	float x;
	float y;
	float radius;
	float elasticity;
	float mass;
	Color color;
	float x_velocity;
	float y_velocity;
	bool fixed;
} Point;


#include "raylib.h"

#pragma once
#ifndef POINT_H
#define POINT_H
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
#endif // POINT_H

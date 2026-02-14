#include "raylib.h"
#include "point.h"

typedef struct {
	Vector2 p1;
	Vector2 p2;
	float thick;
	float strength;
	float original_length;
	Color color;

	bool p1_blocked;
	bool p2_blocked;

	Point *p1_point;
	Point *p2_point;
} Spring;

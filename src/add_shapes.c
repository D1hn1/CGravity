#include "../include/add_shapes.h"

Object add_point(bool is_fixed) {
	int mx = GetMouseX();
	int my = GetMouseY();
	// Create new point
	Point *newPoint = malloc(sizeof(Point));
	*newPoint = (Point){
		.x = mx,
		.y = my,
		//.radius = GetRandomValue(20.0f, 50.0f), // Default 10.0f
		.radius = 15.0f, // Default 10.0f
		.elasticity =  0.6f,
		.color = BLACK,
		.mass = 1.0f, // Default 1.6f
		.y_velocity = 0,
		.fixed = is_fixed,
		.x_velocity = 0,
	};
	// Create new object
	Object newObject = {
		.type = OBJ_POINT,
		.as.point = newPoint,
	};
	printf("Point added x=%d, y=%d\n", mx, my);
	return newObject;
}

Object add_rect(void) {
	int mx = GetMouseX();
	int my = GetMouseY();
	// Create new rect
	Rectangle rec = {
		.x = mx,
		.y = my,
		.width = 50,
		.height = 50,
	};
	Rect *newRect = malloc(sizeof(Rect));
	*newRect = (Rect) {
		.rec = rec,
		.x_velocity = 0,
		.y_velocity = 0,
		.color = BLACK,
		.mass = 1.0f,
	};
	// Create new object
	Object newObject = {
		.type = OBJ_RECT,
		.as.rect = newRect,
	};
	printf("Rectangle added x=%d, y=%d\n", mx, my);
	return newObject;
}

Object add_spring(void) {
	int mx = GetMouseX();
	int my = GetMouseY();
	Vector2 mouse_cords = {mx, my};
	// Create new spring
	Spring *newSpring = malloc(sizeof(Spring));
	*newSpring = (Spring) {
		.p1 = mouse_cords,
		.p2 = mouse_cords,
		.thick = 10.0f,
		.strength = 0.18f,
		.color = BLACK,
		.p1_blocked = false,
		.p2_blocked = false,
	};
	// Create new object
	Object newObject = {
		.type = OBJ_SPRING,
		.as.spring = newSpring,
	};
	printf("Spring added x=%d, y=%d\n", mx, my);
	return newObject;
}

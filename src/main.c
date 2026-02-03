#include <stdio.h>
#include "../include/raylib.h"
#include "../include/raymath.h"
#include "../include/array.h"

// -**- TODO -**-
//
// RESOLVE COLLISIONS WITH RECT - RECT
// RESOLVE COLLISIONS WITH RECT - POINT
// REPEAL THE RECTS

#define FPS 120 
#define WINDOW_TITLE "Gravity on C"

bool  RUNNING = true;
float GRAVITY = 1400.0f;
float FRICTION = 0.977f;
float AIR_FRICTION = 0.98f;
float RESTITUTION_Y = 150.0f;
float RESTITUTION_X = 5.0f;
ObjectType DRAWING = OBJ_RECT;

Object add_point() {
	int mx = GetMouseX();
	int my = GetMouseY();
	// Create new point
	Point *newPoint = malloc(sizeof(Point));
	*newPoint = (Point){
		.x = mx,
		.y = my,
		.radius = GetRandomValue(20.0f, 50.0f), // Default 10.0f
		//.radius = 10.0f, // Default 10.0f
		.elasticity =  0.6f,
		.color = BLACK,
		.mass = 1.0f, // Default 1.6f
		.y_velocity = 0,
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

Object add_rect() {
	int mx = GetMouseX();
	int my = GetMouseY();
	// Create new rect
	Rect *newRect = malloc(sizeof(Rect));
	*newRect = (Rect) {
		.x = mx,
		.y = my,
		.width = 50,
		.height = 50,
		.x_velocity = 0,
		.y_velocity = 0,
		.color = BLACK,
	};
	// Create new object
	Object newObject = {
		.type = OBJ_RECT,
		.as.rect = newRect,
	};
	printf("Rectangle added x=%d, y=%d\n", mx, my);
	return newObject;
}

void update_velocity(Object *object, float delta) {
	if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		actual_rect->y_velocity += GRAVITY * delta;
		actual_rect->y_velocity *= AIR_FRICTION;
	} else {
		// Calculate point velocity
		Point *actual_point = object->as.point; 
		float forceY = actual_point->mass * GRAVITY;
		float accelY = forceY / actual_point->mass;
		// Y - Velocity
		actual_point->y_velocity += GRAVITY * delta;
		actual_point->y_velocity += accelY * delta;
		actual_point->y_velocity *= AIR_FRICTION;
		// X - Velocity
		if (actual_point->y <= (GetScreenHeight() + 10))
			actual_point->x_velocity *= FRICTION;
		if (fabs(actual_point->x_velocity) < RESTITUTION_X)
			actual_point->x_velocity = 0;
	}
}

void apply_velocity(Object *object, float delta) {
	if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		actual_rect->y += actual_rect->y_velocity * delta;
		actual_rect->x += actual_rect->x_velocity * delta;
	} else {
		Point *actual_point = object->as.point;
		actual_point->y += actual_point->y_velocity * delta;
		actual_point->x += actual_point->x_velocity * delta;
	}
}

void check_boundaries(Object *object) {
	// Check boundaries
	Vector2 top_p1   = {0, 0};
	Vector2 top_p2   = {GetScreenWidth(), 0};
	Vector2 floor_p1 = {0, GetScreenHeight()};
	Vector2 floor_p2 = {GetScreenWidth(), GetScreenHeight()};
	Vector2 wallL_p1 = {0, 0};
	Vector2 wallL_p2 = {0, GetScreenHeight()};
	Vector2 wallR_p1 = {GetScreenWidth(), 0};
	Vector2 wallR_p2 = {GetScreenWidth(), GetScreenHeight()};
	// Check objects
	if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		Rectangle floor_rect = {
			.x = 0,
			.y = floor_p1.y,
			.width = top_p2.x,
			.height = 1,
		};
		Rectangle actual_rect_obj = {
			.x = actual_rect->x,
			.y = actual_rect->y,
			.width = actual_rect->width,
			.height = actual_rect->height,
		};
		// Check floor
		if (CheckCollisionRecs(actual_rect_obj, floor_rect)) {
			actual_rect->y = floor_p1.y - actual_rect->height;
		};
	} else {
		Point *actual_point = object->as.point;
		// Vector2 of cords
		Vector2 actual_point_cords = {actual_point->x, actual_point->y};
		// Check floor
		if (CheckCollisionCircleLine(actual_point_cords, actual_point->radius, floor_p1, floor_p2)) {
			actual_point->y = floor_p1.y - actual_point->radius;
			actual_point->y_velocity *= -actual_point->elasticity;
			if (fabs(actual_point->y_velocity) < RESTITUTION_Y)
				actual_point->y_velocity = 0;
		}
		// Check left wall 
		if (CheckCollisionCircleLine(actual_point_cords, actual_point->radius, wallL_p1, wallL_p2)) {
			actual_point->x_velocity *= -actual_point->elasticity;
			actual_point->x = wallL_p1.x + actual_point->radius;
			actual_point->x_velocity = -actual_point->x_velocity * FRICTION;
		}
		// Check right wall
		if (CheckCollisionCircleLine(actual_point_cords, actual_point->radius, wallR_p1, wallR_p2)) {
			actual_point->x_velocity *= -actual_point->elasticity;
			actual_point->x = wallR_p1.x - actual_point->radius;
			actual_point->x_velocity = -actual_point->x_velocity * FRICTION;
		}
		// Check top
		if (CheckCollisionCircleLine(actual_point_cords, actual_point->radius, top_p1, top_p2)) {
			actual_point->y = top_p1.y + actual_point->radius + 5;
			actual_point->y_velocity = -actual_point->y_velocity * actual_point->elasticity;
		}
	}
}

void delete_off_screen(Object *object, darray *points, size_t index) {
	if (object->type == OBJ_POINT) {
		Point *actual_point = object->as.point;
		if ((actual_point->x < 0) || (actual_point->x > GetScreenWidth()) || (actual_point->y < 0) || (actual_point->y > GetScreenHeight()))
			darray_delete(points, index);
	} else if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		if ((actual_rect->x < 0) || (actual_rect->x > GetScreenWidth()) || (actual_rect->y < 0) || (actual_rect->y > GetScreenHeight()))
			darray_delete(points, index);
	}
}

void check_collision(Object *actual_object, Object *inter_object) {
	if ((actual_object->type == OBJ_POINT) && (inter_object->type == OBJ_POINT)) {
		Point *actual_point = actual_object->as.point;
		Point *inter_point  = inter_object->as.point;
		Vector2 delt = {inter_point->x - actual_point->x, inter_point->y - actual_point->y};
		float distance = sqrt(delt.x * delt.x + delt.y * delt.y);
		float overlap = actual_point->radius + inter_point->radius - distance;
		if (overlap > 0) {
			Vector2 normal = { delt.x / distance, delt.y / distance };
			actual_point->x -= 0.5f * overlap * normal.x;
			actual_point->y -= 0.5f * overlap * normal.y;
			inter_point->x  += 0.5f * overlap * normal.x;
			inter_point->y  += 0.5f * overlap * normal.y;

			Vector2 rVelocity = { inter_point->x_velocity - actual_point->x_velocity, inter_point->y_velocity - actual_point->y_velocity };
			float normalVelocity = rVelocity.x * normal.x + rVelocity.y * normal.y;
			if (normalVelocity < 0) {
				float impulse = (-(1 + actual_point->elasticity) * normalVelocity) / 2;
				Vector2 impulseVector = { normal.x * impulse, normal.y * impulse };
				
				actual_point->x_velocity -= impulseVector.x;
				actual_point->y_velocity -= impulseVector.y;
				inter_point->x_velocity += impulseVector.x;
				inter_point->y_velocity += impulseVector.y;
			}
			
		}
	} else if ((actual_object->type == OBJ_RECT) && (inter_object->type == OBJ_RECT)) {
		return;
	} else if ((actual_object->type == OBJ_RECT) && (inter_object->type == OBJ_POINT)) {
		return;
	}
}

void repeal_circle(Point *inter_point) {
	Vector2 inter_point_cords = {inter_point->x, inter_point->y};
	Vector2 mouseV = {GetMouseX(), GetMouseY()};
	if (CheckCollisionCircles(mouseV, 100.0f, inter_point_cords, inter_point->radius)) {
		Vector2 deltM = {inter_point->x - mouseV.x, inter_point->y - mouseV.y};
		float distanceM = sqrt(deltM.x * deltM.x + deltM.y * deltM.y);
		Vector2 normalM = {deltM.x / distanceM, deltM.y / distanceM};
		inter_point->x_velocity += normalM.x * 10.0f;
		inter_point->y_velocity += normalM.y * 10.0f;
	}
}

void draw(Object *object) {
	if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		DrawRectangle(actual_rect->x, actual_rect->y, actual_rect->width, actual_rect->height, actual_rect->color);
	} else {
		Point *actual_point = object->as.point;
		DrawCircle(actual_point->x, actual_point->y, actual_point->radius, actual_point->color);
	}
}

bool menu_shapes() {
	// Get mouse cords
	Vector2 mouse_cords = {GetMouseX(), GetMouseY()};
	// Circle cords
	Vector2 circle_pos = {GetScreenWidth() - 30, 30};
	float circle_radius = 25.0f;
	// Rect cords
	Vector2 rect_siz = {50.0f, 50.0f};
	Vector2 rect_pos = {GetScreenWidth() - 100 - rect_siz.x / 2, 30 - rect_siz.y / 2};
	Rectangle rect = {
		.x = rect_pos.x,
		.y = rect_pos.y,
		.width = rect_siz.x,
		.height = rect_siz.y,
	};
	// Draw them
	DrawCircle(circle_pos.x, circle_pos.y, circle_radius, RED);
	DrawRectangle(rect_pos.x, rect_pos.y, rect_siz.x, rect_siz.y, RED);
	// Check click
	if (CheckCollisionPointCircle(mouse_cords, circle_pos, circle_radius)) {
		DRAWING = OBJ_POINT;
		return true;
	}
	if (CheckCollisionPointRec(mouse_cords, rect)) {
		DRAWING = OBJ_RECT;
		return true;
	}
	return false;
}

int main ()
{
	// Initialize window 
	InitWindow(600, 600, WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FPS);

	// Initialize array
	darray points = darray_init();

	while(RUNNING) {

		BeginDrawing();
		float delta = GetFrameTime();

		for (size_t i = 0; i < points.length; i++) {

			// Get actual point and update it´s velocity
			Object *actual_object = darray_at(&points, i);

			update_velocity(actual_object, delta);

			// Check collisions with walls
			check_boundaries(actual_object);

			// Check collisions with other circles
			for (size_t j = 0; j < points.length; j++) {
				
				Object *inter_object = darray_at(&points, j);
				//Point *inter_point = darray_at(&points, j)->as.point;
				if (j == i) continue;
				check_collision(actual_object, inter_object); // TODO: ADD RECTANGLE
				// Repeal circle if right click is pressed
				//if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				//	repeal_circle(inter_point); // TODO: ADD RECTANGLE

			}

			// Delete off-screen particles
			delete_off_screen(actual_object, &points, i);	

			// Update x & y
			apply_velocity(actual_object, delta);
			
			// N of points
			char str[50];
			sprintf(str, "%zu", points.length);
			DrawText(str, 0, 0, 50, BLACK);
			
			draw(actual_object);

		}
		
		// Menu for selecting shape
		bool selecting_shape = menu_shapes();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if (!selecting_shape) {
				if (DRAWING == OBJ_POINT) {
					Object newPoint = add_point();
					darray_push(&points, newPoint);
				} else {
					Object newRect = add_rect();
					darray_push(&points, newRect);
				}
			}
		}

		ClearBackground(WHITE);
		EndDrawing();

		if (WindowShouldClose() || (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_W))) {
			RUNNING = false;
		}

	}

	darray_destroy(&points);
	CloseWindow();
	return 0;
}

#include <stdio.h>
#include "../include/raylib.h"
#include "../include/raymath.h"
#include "../include/array.h"

// -**- TODO -**-
//
// [ DONE.  ] RESOLVE COLLISIONS WITH RECT - RECT
// [ DOING. ] RESOLVE COLLISIONS WITH RECT - POINT
// [ DOING. ] ADD SPRINGS -> https://github.com/Luke23-45/Coding-a-Spring-Physics-Simulation/blob/main/sp.cc
// [        ] REPEAL THE RECTS

#define FPS 120 
#define WINDOW_TITLE "Gravity on C"

bool  RUNNING = true;
float GRAVITY = 1400.0f;
float FRICTION = 0.977f;
float AIR_FRICTION = 0.98f;
float RESTITUTION_Y = 150.0f;
float RESTITUTION_X = 5.0f;
float DAMPING = 0.16f;
ObjectType DRAWING = OBJ_POINT;

bool IS_FIXED = false;
bool IS_GRAVITY = true;
bool DRAWING_SPRING = false;

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

bool switch_gravity(void) { return !IS_GRAVITY; };
bool switch_fixed(void) { return !IS_FIXED; };

void update_velocity(Object *object, float delta) {
	if (IS_GRAVITY) {
		if (object->type == OBJ_RECT) {
			Rect *actual_rect = object->as.rect;
			float forceY = actual_rect->mass * GRAVITY;
			float accelY = forceY / actual_rect->mass;
			// Y - Velocity
			actual_rect->y_velocity += GRAVITY * delta;
			actual_rect->y_velocity += accelY * delta;
			actual_rect->y_velocity *= AIR_FRICTION;
		} else if (object->type == OBJ_POINT) {
			// Calculate point velocity
			Point *actual_point = object->as.point; 
			if (!actual_point->fixed) {
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
		} else {
			return;
		}
	}
}

void apply_velocity(Object *object, float delta) {
	if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		actual_rect->rec.y += actual_rect->y_velocity * delta;
		actual_rect->rec.x += actual_rect->x_velocity * delta;
	} else if (object->type == OBJ_POINT) {
		Point *actual_point = object->as.point;
		if (!actual_point->fixed) {
			actual_point->y += actual_point->y_velocity * delta;
			actual_point->x += actual_point->x_velocity * delta;
		}
	} else if (object->type == OBJ_SPRING) {
		Spring *actual_spring = object->as.spring;
		if (actual_spring->p1_blocked) {
			actual_spring->p1.x = actual_spring->p1_point->x;
			actual_spring->p1.y = actual_spring->p1_point->y;
		}
		if (actual_spring->p2_blocked) {
			actual_spring->p2.x = actual_spring->p2_point->x;
			actual_spring->p2.y = actual_spring->p2_point->y;
		}
	} else {
		return;
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
		// Check floor
		if (CheckCollisionRecs(actual_rect->rec, floor_rect)) {
			actual_rect->rec.y = floor_p1.y - actual_rect->rec.height;
		};
	} else if (object->type == OBJ_POINT) {
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
	} else {
		return;
	}
}

void delete_off_screen(Object *object, darray *points, size_t index) {
	if (object->type == OBJ_POINT) {
		Point *actual_point = object->as.point;
		if ((actual_point->x < 0) || (actual_point->x > GetScreenWidth()) || (actual_point->y < 0) || (actual_point->y > GetScreenHeight()))
			darray_delete(points, index);
	} else if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		if ((actual_rect->rec.x < 0) || (actual_rect->rec.x > GetScreenWidth()) || (actual_rect->rec.y < 0) || (actual_rect->rec.y > GetScreenHeight()))
			darray_delete(points, index);
	} else if (object->type == OBJ_SPRING) {
		Spring *actual_spring = object->as.spring;
		if ((actual_spring->p1.x < 0) || (actual_spring->p1.y < 0) || (actual_spring->p2.x < 0) || (actual_spring->p2.y < 0)) {
			darray_delete(points, index);
		}
		if ((actual_spring->p1.x > GetScreenWidth()) || (actual_spring->p1.y > GetScreenHeight()) || (actual_spring->p2.x > GetScreenWidth()) || (actual_spring->p2.y > GetScreenHeight())) {
			darray_delete(points, index);
		}
	} else {
		return;
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
			if (!actual_point->fixed) {
				actual_point->x -= 0.5f * overlap * normal.x;
				actual_point->y -= 0.5f * overlap * normal.y;
			}
			if (!inter_point->fixed) {
				inter_point->x  += 0.5f * overlap * normal.x;
				inter_point->y  += 0.5f * overlap * normal.y;
			}

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
		Rect *actual_rect = actual_object->as.rect;
		Rect *inter_rect  = inter_object->as.rect;
		if (CheckCollisionRecs(actual_rect->rec, inter_rect->rec)) {
			inter_rect->rec.y = actual_rect->rec.y - inter_rect->rec.height;
		}
	} else {
		return;
	}
}

void repeal_circle(Object *object) {
	if (object->type == OBJ_POINT) {
		Point *inter_point = object->as.point;
		Vector2 inter_point_cords = {inter_point->x, inter_point->y};
		Vector2 mouseV = {GetMouseX(), GetMouseY()};
		if (CheckCollisionCircles(mouseV, 100.0f, inter_point_cords, inter_point->radius)) {
			Vector2 deltM = {inter_point->x - mouseV.x, inter_point->y - mouseV.y};
			float distanceM = sqrt(deltM.x * deltM.x + deltM.y * deltM.y);
			Vector2 normalM = {deltM.x / distanceM, deltM.y / distanceM};
			inter_point->x_velocity += normalM.x * 10.0f;
			inter_point->y_velocity += normalM.y * 10.0f;
		}
	} else {
		return;
	}
}

void draw(Object *object) {
	if (object->type == OBJ_RECT) {
		Rect *actual_rect = object->as.rect;
		DrawRectangle(actual_rect->rec.x, actual_rect->rec.y, actual_rect->rec.width, actual_rect->rec.height, actual_rect->color);
	} else if (object->type == OBJ_POINT) {
		Point *actual_point = object->as.point;
		DrawCircle(actual_point->x, actual_point->y, actual_point->radius, actual_point->color);
	} else if (object->type == OBJ_SPRING) {
		Spring *actual_spring = object->as.spring;
		DrawLineEx(actual_spring->p1, actual_spring->p2, actual_spring->thick, actual_spring->color);
	} else {
		return;
	}
}

bool menu_shapes(void) {
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
	// Spring cords
	Vector2 springp1 = {GetScreenWidth() - 160, 5};
	Vector2 springp2 = {GetScreenWidth() - 160, 50};
	// Draw them
	Color circlec = RED;
	Color rectanc = RED;
	Color springc = RED;
	if (DRAWING == OBJ_RECT) rectanc = GRAY;
	if (DRAWING == OBJ_POINT) circlec = GRAY;
	if (DRAWING == OBJ_SPRING) springc = GRAY;
	DrawLineEx(springp1, springp2, 10.0f, springc);
	DrawCircle(circle_pos.x, circle_pos.y, circle_radius, circlec);
	DrawRectangle(rect_pos.x, rect_pos.y, rect_siz.x, rect_siz.y, rectanc);
	// Check click
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointCircle(mouse_cords, circle_pos, circle_radius)) {
			DRAWING = OBJ_POINT;
			return true;
		}
		if (CheckCollisionPointRec(mouse_cords, rect)) {
			DRAWING = OBJ_RECT;
			return true;
		}
		if (CheckCollisionPointLine(mouse_cords, springp1, springp2, 5)) {
			DRAWING = OBJ_SPRING;
			return true;
		}
	}
	return false;
}

void spring_logic(Object *object, darray *array) {
	if (object->type == OBJ_SPRING) {
		Spring *actual_spring = object->as.spring;
		if (!actual_spring->p1_blocked || !actual_spring->p2_blocked) {
			// Move p1
			if (!actual_spring->p1_blocked) {
				actual_spring->p1.x = GetMouseX();
				actual_spring->p1.y = GetMouseY();
			}
			// Move p2
			if (!actual_spring->p2_blocked) {
				actual_spring->p2.x = GetMouseX();
				actual_spring->p2.y = GetMouseY();
			}
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (DRAWING == OBJ_SPRING)) {
				for (size_t i = 0; i < array->length; i++) {
					Object *actual_object = darray_at(array, i);
					if (actual_object->type == OBJ_POINT) {

						Point *actual_point = actual_object->as.point;
						Vector2 point_cords = {actual_point->x, actual_point->y};
						if (CheckCollisionPointCircle(actual_spring->p1, point_cords, actual_point->radius) && !actual_spring->p1_blocked) {
							actual_spring->p1_point = actual_point;
							actual_spring->p1_blocked = true;
						} else if (CheckCollisionPointCircle(actual_spring->p2, point_cords, actual_point->radius) && !actual_spring->p2_blocked && actual_spring->p1_blocked) {
							actual_spring->p2_point = actual_point;
							actual_spring->p2_blocked = true;
							DRAWING_SPRING = false;
						}

					}
				}
			}

		}
	} else {
		return;
	}
}

void update_spring(Object *object) {
	// TODO: REFINE THE SPRING FORMULA
	if (IS_GRAVITY) {
		if (object->type == OBJ_SPRING) {

			Spring *actual_spring = object->as.spring;
			if (actual_spring->p1_blocked && actual_spring->p2_blocked) {

				float dx = actual_spring->p2.x - actual_spring->p1.x;
				float dy = actual_spring->p2.y - actual_spring->p1.y;
					
				float distance = hypot(dx, dy);
				float displacement = 10 - distance;

				float force = actual_spring->strength * displacement;
				float fx = (dx / distance) * force;
				float fy = (dy / distance) * force;

				if (actual_spring->p1_blocked) {
					if (!actual_spring->p1_point->fixed) {
						actual_spring->p1_point->x_velocity -= fx - DAMPING;
						actual_spring->p1_point->y_velocity -= fy - DAMPING;
					}
				}

				if (actual_spring->p2_blocked) {
					if (!actual_spring->p2_point->fixed) {
						actual_spring->p2_point->x_velocity += fx - DAMPING;
						actual_spring->p2_point->y_velocity += fy - DAMPING;
					}
				}

			}
		}
	}
}

int main ()
{
	// Initialize window 
	InitWindow(600, 600, WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE); SetTargetFPS(FPS);
	// Initialize array
	darray points = darray_init();

	while(RUNNING) {

		BeginDrawing();
		float delta = GetFrameTime();

		for (size_t i = 0; i < points.length; i++) {

			// Get actual point and update it´s velocity
			Object *actual_object = darray_at(&points, i);

			// Check springs
			spring_logic(actual_object, &points);
			update_spring(actual_object);

			// Update velocity of objects
			update_velocity(actual_object, delta);

			// Check collisions with walls
			check_boundaries(actual_object);

			// Check collisions with other circles
			for (size_t j = 0; j < points.length; j++) {
				
				Object *inter_object = darray_at(&points, j);

				if (j == i) continue;
				check_collision(actual_object, inter_object); // TODO: ADD RECTANGLE (SORT OF)
				// Repeal circle if right click is pressed
				if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
					repeal_circle(inter_object); // TODO: ADD RECTANGLE

			}

			// Delete off-screen particles
			delete_off_screen(actual_object, &points, i);	

			// Update x & y
			apply_velocity(actual_object, delta);
			
			// Draw object
			draw(actual_object);

		}

		// -*-*- Questionable code -*-*-
		// Gravity text
		const char *gravtext = "";
		if (!IS_GRAVITY) {
			gravtext = "(1) GRAVITY: OFF";
		} else {
			gravtext = "(1) GRAVITY: ON";
		}
		// Fixed text
		const char *fixtext = "";
		if (!IS_FIXED) {
			fixtext = "(2) FIXED: OFF";
		} else {
			fixtext = "(2) FIXED: ON";
		}
		// N of points
		char str[50];
		sprintf(str, "%zu", points.length);
		DrawText(str, 30, 10, 30, BLACK);
		DrawText(gravtext, 30, 50, 30, BLACK);
		DrawText(fixtext, 30, 90, 30, BLACK);
		// -*-*- Questionable code -*-*-
		
		// Change gravity
		if (IsKeyPressed(KEY_ONE)) 
			IS_GRAVITY = switch_gravity();
		// Change fixed
		if (IsKeyPressed(KEY_TWO))
			IS_FIXED = switch_fixed();
		
		// Menu for selecting shape
		bool selecting_shape = menu_shapes();
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (!selecting_shape) {
				if (DRAWING == OBJ_POINT) {
					Object newPoint = add_point(IS_FIXED);
					darray_push(&points, newPoint);
				} else if (DRAWING == OBJ_RECT) {
					Object newRect = add_rect();
					darray_push(&points, newRect);
				} else if ((DRAWING == OBJ_SPRING) && !DRAWING_SPRING) {
					DRAWING_SPRING = true;
					Object newSpring = add_spring();
					darray_push(&points, newSpring);
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

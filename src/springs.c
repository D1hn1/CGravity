#include "../include/springs.h"

void spring_logic(Object *object, darray *array) {
	if (object->type == OBJ_SPRING) {
		Spring *actual_spring = object->as.spring;
		if (!actual_spring->p1_blocked || !actual_spring->p2_blocked) {
			// If they are not blocked check collisions		
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
							// Calculate original length
							float dx = actual_spring->p1.x - actual_spring->p2.x;
							float dy = actual_spring->p1.y - actual_spring->p2.y;
							float original = hypot(dx, dy);
							actual_spring->original_length = original;
						}

					}
				}
			}

		}
	} else {
		return;
	}
}

void update_spring(Object *object, float dt) {
	if (object->type == OBJ_SPRING) {
		// Get actual spring
		Spring *actual_spring = object->as.spring;
		if (actual_spring->p1_blocked && actual_spring->p2_blocked) {
			if (IS_GRAVITY) {

				Vector2 direction = Vector2Subtract(actual_spring->p1, actual_spring->p2);
				float distance = Vector2Length(direction);
				float x = actual_spring->original_length - distance;

				direction.x *= actual_spring->strength * x;
				direction.y *= actual_spring->strength * x;

				if (actual_spring->p1_blocked) {
					if (!actual_spring->p1_point->fixed) {
						actual_spring->p1_point->x_velocity += direction.x * dt;
						actual_spring->p1_point->y_velocity += direction.y * dt;
						actual_spring->p1_point->x_velocity *= DAMPING;
						actual_spring->p1_point->y_velocity *= DAMPING;
					}
				}
				
				direction = Vector2Negate(direction);
				
				if (actual_spring->p2_blocked) {
					if (!actual_spring->p2_point->fixed) {
						actual_spring->p2_point->x_velocity += direction.x * dt;
						actual_spring->p2_point->y_velocity += direction.y * dt;
						actual_spring->p2_point->x_velocity *= DAMPING;
						actual_spring->p2_point->y_velocity *= DAMPING;
					}
				}

				

			}

		} else {
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
		}
	}
}

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
	if (object->type == OBJ_SPRING) {
		// Get actual spring
		Spring *actual_spring = object->as.spring;
		if (actual_spring->p1_blocked && actual_spring->p2_blocked) {
			if (IS_GRAVITY) {
				float dx = actual_spring->p2.x - actual_spring->p1.x;
				float dy = actual_spring->p2.y - actual_spring->p1.y;
					
				float distance = hypot(dx, dy);
				// TODO: INTRODUCE ORIGINAL LENGTH ./springs.c:29 <- where the code should be
				// 		 REPLACING THE "100" WITH THE LENGTH
				// 		 FROM THE P1 TO P2 INSIDE THE SPRING OBJ.
				float displacement = 100 - distance;

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

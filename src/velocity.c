#include "../include/velocity.h"

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

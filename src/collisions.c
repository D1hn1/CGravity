#include "../include/collisions.h"

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
			//actual_point->x_velocity = -actual_point->x_velocity * FRICTION;
		}
		// Check right wall
		if (CheckCollisionCircleLine(actual_point_cords, actual_point->radius, wallR_p1, wallR_p2)) {
			actual_point->x_velocity *= -actual_point->elasticity;
			actual_point->x = wallR_p1.x - actual_point->radius;
			//actual_point->x_velocity = -actual_point->x_velocity * FRICTION;
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
			if (DRAWING_SPRING) DRAWING_SPRING = false;
		}
		if ((actual_spring->p1.x > GetScreenWidth()) || (actual_spring->p1.y > GetScreenHeight()) || (actual_spring->p2.x > GetScreenWidth()) || (actual_spring->p2.y > GetScreenHeight())) {
			darray_delete(points, index);
			if (DRAWING_SPRING) DRAWING_SPRING = false;
		}
	} else {
		return;
	}
}

void delete_on_cursor(darray *array, Vector2 mcords) {
	for (size_t i = 0; i < array->length; i++) {
		Object *object = darray_at(array, i);
		if (object->type == OBJ_POINT) {
			Point *point = object->as.point;
			Vector2 point_cords = {point->x, point->y};
			if (CheckCollisionPointCircle(mcords, point_cords, point->radius)) {
				darray_delete(array, i);
			}

		} else if (object->type == OBJ_RECT) {
			Rect *rect = object->as.rect;
			if (CheckCollisionPointRec(mcords, rect->rec)) {
				darray_delete(array, i);
			}

		} else if (object->type == OBJ_SPRING) {
			Spring *spring = object->as.spring;
			if (CheckCollisionPointLine(mcords, spring->p1, spring->p2, 5)) {
				darray_delete(array, i);
			}

		} else {
			return;
		}
	}
};

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
		if (!inter_point->fixed) {
			Vector2 inter_point_cords = {inter_point->x, inter_point->y};
			Vector2 mouseV = {GetMouseX(), GetMouseY()};
			if (CheckCollisionCircles(mouseV, 100.0f, inter_point_cords, inter_point->radius)) {
				Vector2 deltM = {inter_point->x - mouseV.x, inter_point->y - mouseV.y};
				float distanceM = sqrt(deltM.x * deltM.x + deltM.y * deltM.y);
				Vector2 normalM = {deltM.x / distanceM, deltM.y / distanceM};
				inter_point->x_velocity += normalM.x * 100.0f;
				inter_point->y_velocity += normalM.y * 100.0f;
			}
		}
	} else {
		return;
	}
}

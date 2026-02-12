#include "../include/drawing.h"

bool switch_gravity(void) { return !IS_GRAVITY; };
bool switch_fixed(void) { return !IS_FIXED; };

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

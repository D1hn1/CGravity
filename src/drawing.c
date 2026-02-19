#include "../include/drawing.h"

bool switch_gravity(void) { return !IS_GRAVITY; };
bool switch_fixed(void) { return !IS_FIXED; };
bool switch_cursor_lines(void) { return !CURSOR_LINES; };

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
	Vector2 springp1 = {GetScreenWidth() - 160, rect.y};
	Vector2 springp2 = {GetScreenWidth() - 160, (rect.y + rect.height)};
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

void draw_info(size_t npoints) {
	// Define vars
	char str[50];
	sprintf(str, "%zu", npoints); 
	const char *gravtext = "(1) GRAVITY: ON";
	const char *fixtext = "(2) FIXED: ON";
	const char *linestext = "(3) LINES: ON";
	// Change bool
	if (!IS_GRAVITY)
		gravtext = "(1) GRAVITY: OFF";
	if (!IS_FIXED)
		fixtext = "(2) FIXED: OFF";
	if (!CURSOR_LINES)
		linestext = "(3) LINES: OFF";
	// Draw text
	DrawText(str, 10, 10, 30, BLACK);
	DrawText(gravtext, 10, 50, 30, BLACK);
	DrawText(fixtext, 10, 90, 30, BLACK);
	DrawText(linestext, 10, 130, 30, BLACK);
}

void draw_cursor_lines(void) {
	Color line_color = RED;
	DrawLine(GetMouseX(), GetMouseY(), 0, GetMouseY(), line_color);
	DrawLine(GetMouseX(), GetMouseY(), GetMouseX(), 0, line_color);
	DrawLine(GetMouseX(), GetMouseY(), GetScreenWidth(), GetMouseY(), line_color);
	DrawLine(GetMouseX(), GetMouseY(), GetMouseX(), GetScreenHeight(), line_color);
}

void draw_debug(darray *array, size_t object) {
	switch (object) {
		case 1: {
			// Anchors
			for (size_t i = 0; i < 2; i++) {
				Point *new_point = malloc(sizeof(Point));
				*new_point = (Point) {
					.x = (i + 5) * 60,
					.y = 300,
					.color = BLACK,
					.elasticity = 0,
					.mass = 0,
					.radius = 15.0f,
					.fixed = true,
				};
				Object newObject = {
					.type = OBJ_POINT,
					.as.point = new_point,
				};
				darray_push(array, newObject);
			}
			// Free points
			for (size_t i = 0; i < 2; i++) {
				Point *new_point = malloc(sizeof(Point));
				*new_point = (Point) {
					.x = (i + 5) * 60,
					.y = 400,
					.color = BLACK,
					.elasticity = 0.6f,
					.mass = 1.0f,
					.radius = 15.0f,
					.fixed = false,
				};
				Object newObject = {
					.type = OBJ_POINT,
					.as.point = new_point,
				};
				darray_push(array, newObject);
			}
			// Springs
			Spring *first = malloc(sizeof(Spring));
			*first = (Spring) {
				.p1_blocked = true,
				.p2_blocked = true,
				.p1_point = darray_at(array, 0)->as.point,
				.p2_point = darray_at(array, 2)->as.point,
				.color = BLACK,
				.strength = 1.0f,
				.thick = 5.0f,
				.original_length = 100,
			};
			Object newSpring = {
				.type = OBJ_SPRING,
				.as.spring = first,
			};
			darray_push(array, newSpring);
			Spring *second = malloc(sizeof(Spring));
			*second = (Spring) {
				.p1_blocked = true,
				.p2_blocked = true,
				.p1_point = darray_at(array, 1)->as.point,
				.p2_point = darray_at(array, 3)->as.point,
				.color = BLACK,
				.strength = 1.0f,
				.thick = 5.0f,
				.original_length = 100,
			};
			Object otherSpring = {
				.type = OBJ_SPRING,
				.as.spring = second,
			};
			darray_push(array, otherSpring);
			Spring *third = malloc(sizeof(Spring));
			*third = (Spring) {
				.p1_blocked = true,
				.p2_blocked = true,
				.p1_point = darray_at(array, 2)->as.point,
				.p2_point = darray_at(array, 3)->as.point,
				.color = BLACK,
				.strength = 1.0f,
				.thick = 5.0f,
				.original_length = 100,
			};
			Object anotherSpring = {
				.type = OBJ_SPRING,
				.as.spring = third,
			};
			darray_push(array, anotherSpring);
		}

		default:
			break;
	}
}

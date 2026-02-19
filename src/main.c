#include <stdio.h>
#include "../include/raylib.h"
#include "../include/raymath.h"
#include "../include/array.h"
#include "../include/add_shapes.h"
#include "../include/global.h"
#include "../include/velocity.h"
#include "../include/collisions.h"
#include "../include/drawing.h"
#include "../include/springs.h"

// -**- TODO -**-
//
// [        ] REPEAL THE RECTS.
// [        ] RESOLVE COLLISIONS WITH RECT - POINT.
// [ DONE.  ] CREATE A FUNCTION TO CREATE A DEBUG STRUCTURE.
// [ DONE.  ] REFINE THE SPRING FORMULA
// [ DONE.  ] FIX THE REPEAL FUNCTION THAT DOESN´T REPEAL THE POINTS WITH 0 X&Y VELOCITY. ( IT WORKS WHEN THERE ARE TWO POINTS )
// [ DONE.  ] ADD A MECHANISM TO DELETE OBJECTS ON SCREEN.
// [ DONE.  ] DRAW LINES FROM THE WALLS TO THE CURSOR.
// [ DONE?. ] ADD SPRINGS -> https://github.com/Luke23-45/Coding-a-Spring-Physics-Simulation/blob/main/sp.cc.
// [ DONE.  ] RESOLVE COLLISIONS WITH RECT - RECT.
// [ DONE.  ] FIX THE R&L WALLS DOESN´T BOUNCE FOR SOME REASON.
// [ DONE?. ] FIX THE ACUMULATION OF ENERGY ON ANCHOR POINTS.

#define FPS 120 
#define WINDOW_TITLE "Gravity on C"

int main ()
{
	// Initialize window 
	InitWindow(600, 600, WINDOW_TITLE);
	SetWindowState(FLAG_WINDOW_RESIZABLE); SetTargetFPS(FPS);
	// Initialize array
	darray points = darray_init();
	
	if (false) {
		// Draw debug structure
		draw_debug(&points, 1);
	}

	while(RUNNING) {

		BeginDrawing();
		float delta = GetFrameTime();

		for (size_t i = 0; i < points.length; i++) {

			// Get actual point and update it´s velocity
			Object *actual_object = darray_at(&points, i);

			// Check springs
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
				
			}

			// Repeal circle if right click is pressed
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				repeal_circle(actual_object); // TODO: ADD RECTANGLE

			// Delete off-screen particles
			delete_off_screen(actual_object, &points, i);	

			// Update x & y
			apply_velocity(actual_object, delta);
			
			// Draw object
			draw(actual_object);

		}

		// Draw cursor lines
		if (CURSOR_LINES) 
			draw_cursor_lines();

		// Draw top left info
		draw_info(points.length);
		
		// Change gravity
		if (IsKeyPressed(KEY_ONE)) 
			IS_GRAVITY = switch_gravity();
		// Change fixed
		if (IsKeyPressed(KEY_TWO))
			IS_FIXED = switch_fixed();
		// Change cursor lines
		if (IsKeyPressed(KEY_THREE))
			CURSOR_LINES = switch_cursor_lines();
		
		// Menu for selecting shape
		bool selecting_shape = menu_shapes();
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT)) {
			if (!selecting_shape) {
				if (DRAWING == OBJ_POINT && !DRAWING_SPRING) {
					Object newPoint = add_point(IS_FIXED);
					darray_push(&points, newPoint);
				} else if (DRAWING == OBJ_RECT && !DRAWING_SPRING) {
					Object newRect = add_rect();
					darray_push(&points, newRect);
				} else if (DRAWING == OBJ_SPRING) {
					// Create new Spring
					if (!DRAWING_SPRING) {
						Object newSpring = add_spring();
						darray_push(&points, newSpring);
					}
					DRAWING_SPRING = true;
					// Check for snapping to a point
					Object *last_object = darray_at(&points, points.length - 1);
					spring_logic(last_object, &points);
				}
			}
		} else if (IsKeyDown(KEY_LEFT_SHIFT) && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !DRAWING_SPRING) {
			// Get mouse cords
			Vector2 mcords = {GetMouseX(), GetMouseY()};
			// Delete on mouse objects
			delete_on_cursor(&points, mcords);
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

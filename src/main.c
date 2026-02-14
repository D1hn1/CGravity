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
// [ DOING. ] ADD SPRINGS -> https://github.com/Luke23-45/Coding-a-Spring-Physics-Simulation/blob/main/sp.cc.
// [ DONE.  ] RESOLVE COLLISIONS WITH RECT - RECT.
// [        ] RESOLVE COLLISIONS WITH RECT - POINT.
// [        ] REPEAL THE RECTS.
// [        ] ADD A MECHANISM TO DELETE OBJECTS ON SCREEN.
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

		// Draw top left info
		draw_info(points.length);
		
		// Change gravity
		if (IsKeyPressed(KEY_ONE)) 
			IS_GRAVITY = switch_gravity();
		// Change fixed
		if (IsKeyPressed(KEY_TWO))
			IS_FIXED = switch_fixed();
		
		// Menu for selecting shape
		// TODO: Fix so the spring draws on the first click.
		// 		 Right now it creates on the first and then draws on the second becouse
		// 		 	of the checkbox collision detection.
		// 		 Fix the problem that you can not create another spring if one is out
		// 		 	of screen or bugged out.
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

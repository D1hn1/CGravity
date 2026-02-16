#include <stdio.h>
#include "array.h"
#include "global.h"

#ifndef DRAWING_H
#define DRAWING_H
bool switch_gravity(void);
bool switch_fixed(void);
bool switch_cursor_lines(void);
void draw(Object *object);
bool menu_shapes(void);
void draw_info(size_t npoints);
void draw_cursor_lines();
#endif // DRAWING_H

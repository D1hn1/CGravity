#include <stdio.h>
#include "array.h"
#include "global.h"

#ifndef DRAWING_H
#define DRAWING_H
bool switch_gravity(void);
bool switch_fixed(void);
void draw(Object *object);
bool menu_shapes(void);
#endif // DRAWING_H

#include <stdio.h>
#include "array.h"
#include "global.h"
#include "raymath.h"

#ifndef COLLISION_H
#define COLLISION_H
void check_boundaries(Object *object);
void delete_off_screen(Object *object, darray *points, size_t index);
void check_collision(Object *actual_object, Object *inter_object);
void repeal_circle(Object *object);
#endif // COLLISION_H

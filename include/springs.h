#include <stdio.h>
#include "array.h"
#include "global.h"
#include "raymath.h"

#ifndef SPRINGS_H
#define SPRINGS_H
void spring_logic(Object *object, darray *array);
void update_spring(Object *object, float dt);
#endif // SPRINGS_H

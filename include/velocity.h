#include <stdio.h>
#include "array.h"
#include "raymath.h"
#include "global.h"

#ifndef VELOCITY_H
#define VELOCITY_H
void update_velocity(Object *object, float delta);
void apply_velocity(Object *object, float delta);
#endif // VELOCITY_H

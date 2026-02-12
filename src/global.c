#include "../include/global.h"

bool  RUNNING = true;
float GRAVITY = 1400.0f;
float FRICTION = 0.977f;
float AIR_FRICTION = 0.98f;
float RESTITUTION_Y = 150.0f;
float RESTITUTION_X = 5.0f;
float DAMPING = 0.16f;
ObjectType DRAWING = OBJ_POINT;

bool IS_FIXED = false;
bool IS_GRAVITY = true;
bool DRAWING_SPRING = false;

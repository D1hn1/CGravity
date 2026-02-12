#include <stdlib.h>
#include "point.h"
#include "rect.h"
#include "spring.h"

typedef enum {
	OBJ_POINT,
	OBJ_RECT,
	OBJ_SPRING,
} ObjectType;

typedef struct {
	ObjectType type;
	union {
		Point *point;
		Rect  *rect;
		Spring *spring;
	} as;
} Object;

typedef struct {
	Object *data;
	size_t capacity;
	size_t length;
} darray;

darray darray_init();
Object *darray_at(darray *self, size_t i);
void darray_push(darray *self, Object item);
void darray_pop(darray *self);
void darray_delete(darray *self, size_t i);
void darray_destroy(darray *self);

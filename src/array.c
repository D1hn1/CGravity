#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/array.h"

darray darray_init() {
	darray self;
	self.capacity = 10;
	self.length = 0;
	self.data = calloc(10, sizeof(Object));

	return self;
}

Object *darray_at(darray *self, size_t i) {
	if (i < self->length) {
		return &self->data[i];
	}
	return NULL;
}

void darray_push(darray *self, Object item) {
	if (self->length >= self->capacity) {
		self->capacity += 2;
		Object *tmp = realloc(self->data, self->capacity * sizeof(Object));
		if (!tmp) return;
		self->data = tmp;
	}
	self->data[self->length] = item;
	self->length++;
}

void darray_pop(darray *self) {
	if (self->length > 0) {
		self->length--;
	}
}

void darray_delete(darray *self, size_t i) {
	if (i >= self->length) return;
	memmove(
		self->data + i,
		self->data + i + 1,
		(self->length - i - 1) * sizeof(Object)
	);
	self->length--;
}

void darray_destroy(darray *self) {
	free(self->data);
	self->data = NULL;
	self->length = 0;
	self->capacity = 0;
}

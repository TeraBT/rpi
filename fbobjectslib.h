#ifndef FBOBJECTSLIB_H
#define FBOBJECTSLIB_H

#include <stdint.h>
#include <stdlib.h>

#include "fblib.h"

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define INIT_OBJECT(coords) ((struct object){coords, ARRAY_LEN(coords)})

struct object {
  int *coords;
  size_t len;
};

struct object *create_object(size_t x_start, size_t y_start, size_t x_end,
                             size_t y_end);

struct object *create_square(size_t x_start, size_t y_start, size_t size);

struct object *create_circle(int x_start, int y_start, int radius);

void paint_object(struct framebuffer *fb, struct object *object,
                  uint16_t color);

void shift_object(struct object *object, int shift_x, int shift_y);

int get_center_x(struct object *object);

int get_center_y(struct object *object);

size_t get_distance(struct object *object1, struct object *object2);

#endif

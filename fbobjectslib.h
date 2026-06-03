#ifndef FBOBJECTSLIB_H
#define FBOBJECTSLIB_H

#include <stdint.h>
#include <stdlib.h>

#include "fblib.h"

typedef struct coord {
  int x;
  int y;
} coord;

typedef struct object {
  coord *coords;
  size_t len;
  coord center;
} object;

object *create_object(size_t x_start, size_t y_start, size_t x_end,
                      size_t y_end);

object *create_square(size_t x_start, size_t y_start, size_t size);

object *create_circle(int x_start, int y_start, int radius);

void paint_object(struct framebuffer *fb, object *object, uint16_t color);

void shift_object(object *object, int shift_x, int shift_y);

size_t get_distance(object *object1, object *object2);

#endif

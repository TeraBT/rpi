#ifndef FBOBJECTSLIB_H
#define FBOBJECTSLIB_H

#include <stdint.h>
#include <stdlib.h>

#include "fblib.h"

typedef struct coord {
  double x;
  double y;
} coord;

typedef struct object {
  coord *coords;
  size_t len;
  coord center;
} object;

typedef struct vector {
  double x;
  double y;
} vector;

object *create_object(size_t x_start, size_t y_start, size_t x_end,
                      size_t y_end);

object *create_square(size_t x_start, size_t y_start, size_t size);

object *create_circle(int x_start, int y_start, int radius);

void draw_object(struct framebuffer *fb, object *object, uint16_t color);

void shift_object(object *object, int shift_x, int shift_y);

void rotate_object(object *object, double degrees);

size_t compute_distance(object *object1, object *object2);

vector compute_directional_vector(coord coord1, coord coord2);

double compute_gravitational_force(object *object1, object *object2);

vector compute_gravitational_pull(object *object1, object *object2);

uint16_t compute_bgr_heatmap(double t);

#endif

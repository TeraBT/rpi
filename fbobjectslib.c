#include "fbobjectslib.h"

#include <math.h>
#include <stdint.h>

#include "fblib.h"

struct object *create_object(size_t x_start, size_t y_start, size_t x_end,
                             size_t y_end) {

  int coord_count = (x_end - x_start + 1) * (y_end - y_start + 1);
  int element_count = coord_count * 2;

  struct object *object = malloc(sizeof(struct object));
  if (!object)
    return NULL;

  object->coords = malloc(element_count * sizeof(int));
  if (!object->coords) {
    free(object);
    return NULL;
  }
  object->len = element_count;

  int k = 0;
  for (size_t j = y_start; j <= y_end; j++) {
    for (size_t i = x_start; i <= x_end; i++) {
      object->coords[k++] = i;
      object->coords[k++] = j;
    }
  }

  return object;
}

struct object *create_square(size_t x_start, size_t y_start, size_t size) {
  return create_object(x_start, y_start, x_start + size, y_start + size);
}

struct object *create_circle(int x_start, int y_start, int radius) {

  int square_side_len = (radius * 2 + 1);
  int square_coords_count = square_side_len * square_side_len;
  int element_count = 2 * square_coords_count;

  struct object *object = malloc(sizeof(struct object));
  if (!object)
    return NULL;

  object->coords = malloc(element_count * sizeof(int));
  if (!object->coords) {
    free(object);
    return NULL;
  }

  int k = 0;
  for (int y = y_start - radius; y <= y_start + radius; y++) {
    for (int x = x_start - radius; x <= x_start + radius; x++) {
      int dx = x - x_start;
      int dy = y - y_start;
      if (dx * dx + dy * dy <= radius * radius) {
        object->coords[k++] = x;
        object->coords[k++] = y;
      }
    }
  }

  object->len = k;

  object->coords = realloc(object->coords, k * sizeof(int));
  if (!object->coords) {
    free(object);
    return NULL;
  }

  return object;
}

void paint_object(struct framebuffer *fb, struct object *object,
                  uint16_t color) {
  for (size_t i = 0; i < object->len; i += 2) {
    paint_pixel(fb, object->coords[i], object->coords[i + 1], color);
  }
}

void shift_object(struct object *object, int shift_x, int shift_y) {
  for (size_t i = 0; i < object->len; i += 2) {
    object->coords[i] += shift_x;
  }

  for (size_t i = 1; i < object->len; i += 2) {
    object->coords[i] += shift_y;
  }
}

int get_center_x(struct object *object) {
  int x_acc = 0;
  for (size_t i = 0; i < object->len; i += 2) {
    x_acc += object->coords[i];
  }
  return x_acc / (object->len / 2);
}

int get_center_y(struct object *object) {
  int y_acc = 0;
  for (size_t i = 1; i < object->len; i += 2) {
    y_acc += object->coords[i];
  }
  return y_acc / (object->len / 2);
}

size_t get_distance(struct object *object1, struct object *object2) {
  int object1_center_x = get_center_x(object1);
  int object1_center_y = get_center_y(object1);

  int object2_center_x = get_center_x(object2);
  int object2_center_y = get_center_y(object2);

  int dx = object2_center_x - object1_center_x;
  int dy = object2_center_y - object1_center_y;
  return sqrt(dx * dx + dy * dy);
}
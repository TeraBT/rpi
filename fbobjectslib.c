#include "fbobjectslib.h"

#include <math.h>
#include <stdint.h>

#include "fblib.h"

static coord compute_center(struct object *object);

struct object *create_object(size_t x_start, size_t y_start, size_t x_end,
                             size_t y_end) {

  int coord_count = (x_end - x_start + 1) * (y_end - y_start + 1);

  struct object *object = malloc(sizeof(struct object));
  if (!object)
    return NULL;

  object->coords = malloc(coord_count * sizeof(coord));
  if (!object->coords) {
    free(object);
    return NULL;
  }
  object->len = coord_count;

  int k = 0;
  for (size_t y = y_start; y <= y_end; y++) {
    for (size_t x = x_start; x <= x_end; x++) {
      object->coords[k++] = (coord){x, y};
    }
  }

  object->center = compute_center(object);
  return object;
}

struct object *create_square(size_t x_start, size_t y_start, size_t size) {
  return create_object(x_start, y_start, x_start + size, y_start + size);
}

struct object *create_circle(int x_start, int y_start, int radius) {

  int square_side_len = (radius * 2 + 1);
  int square_coord_count = square_side_len * square_side_len;

  struct object *object = malloc(sizeof(struct object));
  if (!object)
    return NULL;

  object->coords = malloc(square_coord_count * sizeof(struct coord));
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
        object->coords[k++] = (coord){x, y};
      }
    }
  }

  object->len = k;

  object->coords = realloc(object->coords, k * sizeof(struct coord));
  if (!object->coords) {
    free(object);
    return NULL;
  }

  object->center = compute_center(object);
  return object;
}

void draw_object(struct framebuffer *fb, struct object *object,
                 uint16_t color) {
  for (size_t i = 0; i < object->len; i++) {
    paint_pixel(fb, round(object->coords[i].x), round(object->coords[i].y),
                color);
  }
}

void shift_object(struct object *object, int shift_x, int shift_y) {
  for (size_t i = 0; i < object->len; i++) {
    object->coords[i].x += shift_x;
    object->coords[i].y += shift_y;
  }
  object->center.x += shift_x;
  object->center.y += shift_y;
}

static coord compute_center(struct object *object) {
  int x_acc = 0;
  int y_acc = 0;
  for (size_t i = 0; i < object->len; i++) {
    x_acc += object->coords[i].x;
    y_acc += object->coords[i].y;
  }
  return (coord){x_acc / object->len, y_acc / object->len};
}

size_t compute_distance(struct object *object1, struct object *object2) {
  int dx = object2->center.x - object1->center.x;
  int dy = object2->center.y - object1->center.y;
  return sqrt(dx * dx + dy * dy);
}

vector compute_directional_vector(coord from, coord to) {
  vector unnormalized = {to.x - from.x, to.y - from.y};
  double magnitude =
      sqrt(unnormalized.x * unnormalized.x + unnormalized.y * unnormalized.y);
  vector normalized = {unnormalized.x / magnitude, unnormalized.y / magnitude};
  return normalized;
}

#define MASS_FACTOR 1e4
static const double GRAVITATIONAL_CONSTANT = 6.674e-11;

double compute_gravitational_force(object *object1, object *object2) {
  int dist = compute_distance(object1, object2);
  return object1->len * MASS_FACTOR * object2->len * MASS_FACTOR /
         (dist * dist) * GRAVITATIONAL_CONSTANT;
}

vector compute_gravitational_pull(object *object1, object *object2) {
  double gravitational_force = compute_gravitational_force(object1, object2);
  vector directional_vector =
      compute_directional_vector(object1->center, object2->center);
  return (vector){directional_vector.x * gravitational_force,
                  directional_vector.y * gravitational_force};
}

uint16_t compute_bgr_heatmap(double t) {

  uint8_t r, g, b;
  if (t < 0.5) {
    double u = t * 2;
    b = 255 * (1 - u);
    g = 255 * u;
    r = 0;
  } else {
    double u = (t - 0.5) * 2;
    b = 0;
    g = 255 * (1 - u);
    r = 255 * u;
  }

  return get_color(r, g, b);
}
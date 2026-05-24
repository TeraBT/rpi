#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "fblib.h"

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define INIT_OBJECT(coords) ((struct object){coords, ARRAY_LEN(coords)})

static struct termios current_terminal_settings;

void enable_raw_mode(void) {
  struct termios t;

  tcgetattr(STDIN_FILENO, &current_terminal_settings);
  t = current_terminal_settings;
  t.c_lflag &= ~ICANON & ~ECHO;

  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void disable_raw_mode(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &current_terminal_settings);
}

struct object {
  int *coords;
  size_t len;
};

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

void *rand_thread(void *args) {
  struct framebuffer framebuffer;
  struct framebuffer *fb = &framebuffer;
  open_fb(fb);

  uint16_t white = get_color(255, 255, 255);
  uint16_t black = get_color(0, 0, 0);
  uint16_t red = get_color(255, 0, 0);
  uint16_t green = get_color(0, 255, 0);
  uint16_t blue = get_color(0, 0, 255);

  int x_start = 1000;
  int y_start = 500;
  struct object *square =
      create_object(x_start, y_start, x_start + 20, y_start + 20);

  paint_object(fb, square, white);

  int min = -1;
  int max = 1;
  int step_size = 20;

  while (1) {
    paint_object(fb, square, black);

    int r_x = min + rand() % (max - min + 1);
    int r_y = min + rand() % (max - min + 1);

    int x = r_x;
    if (r_x == -1)
      x = -step_size;
    if (r_x == 1)
      x = step_size;

    int y = r_y;
    if (r_y == -1)
      y = -step_size;
    if (r_y == 1)
      y = step_size;

    shift_object(square, x, y);

    uint16_t color = red;
    if (r_x == -1)
      color = green;
    if (r_x == 1)
      color = blue;

    paint_object(fb, square, color);
    usleep(200000);
  }
}

int main(void) {
  enable_raw_mode();
  srand(time(NULL));

  int thread_count = 400;
  // pthread_t *thread_arr = malloc(thread_count * sizeof(pthread_t));

  for (size_t i = 0; i < thread_count; i++) {
    pthread_t t;
    // thread_arr[i] = t;
    pthread_create(&t, NULL, rand_thread, NULL);
  }

  struct framebuffer framebuffer;
  struct framebuffer *fb = &framebuffer;
  open_fb(fb);

  uint16_t white = get_color(255, 255, 255);
  uint16_t red = get_color(255, 0, 0);
  uint16_t black = get_color(0, 0, 0);

  int line_coords[] = {0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0};

  struct object *line = &INIT_OBJECT(line_coords);

  int x_start = 1000;
  int y_start = 500;
  struct object *square =
      create_object(x_start, y_start, x_start + 20, y_start + 20);

  // shift_object(square, 1000, 500);
  paint_object(fb, square, white);

  while (1) {
    int c = getchar();

    if (c == 'q')
      break;
    else if (c == 'h') {
      paint_object(fb, square, black);
      shift_object(square, -5, 0);
    } else if (c == 'l') {
      paint_object(fb, square, black);
      shift_object(square, 5, 0);
    } else if (c == 'k') {
      paint_object(fb, square, black);
      shift_object(square, 0, -5);
    } else if (c == 'j') {
      paint_object(fb, square, black);
      shift_object(square, 0, 5);
    }

    paint_object(fb, square, white);
  }

  free(square);
  // free(thread_arr);  pthread_create(&t0, NULL, rand_thread, NULL);

  disable_raw_mode();
}
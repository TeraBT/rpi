#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "fblib.h"
#include "fbobjectslib.h"

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

#define THREAD_COUNT 100
struct object *thread_objects[THREAD_COUNT];

struct object *movable_object;

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }

void *random_walk(void *args);
void *static_dist(void *args);
void *random_walk_dist(void *args);

int main(void) {
  enable_raw_mode();

  system("echo 0 | sudo tee /sys/class/graphics/fbcon/cursor_blink >/dev/null");
  system("tput clear | sudo tee /dev/tty1 > /dev/null");

  srand(time(NULL));

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
  struct object *square = create_square(x_start, y_start, 20);
  movable_object = square;

  paint_object(fb, square, white);

  size_t thread_indices[THREAD_COUNT];

  for (size_t i = 0; i < THREAD_COUNT; i++) {
    pthread_t t;
    thread_indices[i] = i;
    pthread_create(&t, NULL, random_walk_dist, &thread_indices[i]);
  }

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
  close_fb(fb);

  disable_raw_mode();
}

void *random_walk_dist(void *args) {
  struct framebuffer framebuffer;
  struct framebuffer *fb = &framebuffer;
  open_fb(fb);

  uint16_t white = get_color(255, 255, 255);
  uint16_t black = get_color(0, 0, 0);
  uint16_t red = get_color(255, 0, 0);
  uint16_t green = get_color(0, 255, 0);
  uint16_t blue = get_color(0, 0, 255);

  int x_start = rand_range(800, 1200);
  int y_start = rand_range(300, 700);
  struct object *circle = create_circle(x_start, y_start, 20);
  size_t thread_index = *(size_t *)args;
  thread_objects[thread_index] = circle;

  paint_object(fb, circle, white);

  int min = -1;
  int max = 1;
  int step_size = 10;

  while (1) {
    paint_object(fb, circle, black);

    int r_x = rand_range(min, max);
    int r_y = rand_range(min, max);

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

    shift_object(circle, x, y);

    size_t distance = get_distance(circle, movable_object);
    printf("Distance of object %ld to movable object is %ld.\n", thread_index,
           distance);

    uint16_t color = distance < 100 ? red : blue;
    paint_object(fb, circle, color);
    usleep(200000);
  }
}

void *static_dist(void *args) {
  struct framebuffer framebuffer;
  struct framebuffer *fb = &framebuffer;
  open_fb(fb);

  uint16_t white = get_color(255, 255, 255);
  uint16_t black = get_color(0, 0, 0);
  uint16_t red = get_color(255, 0, 0);
  uint16_t green = get_color(0, 255, 0);
  uint16_t blue = get_color(0, 0, 255);

  int x_start = rand_range(800, 1200);
  int y_start = rand_range(300, 700);
  struct object *circle = create_circle(x_start, y_start, 20);
  size_t thread_index = *(size_t *)args;
  thread_objects[thread_index] = circle;

  paint_object(fb, circle, white);

  while (1) {
    paint_object(fb, circle, black);

    size_t distance = get_distance(circle, movable_object);
    printf("Distance of object %ld to movable object is %ld.\n", thread_index,
           distance);

    uint16_t color = distance < 100 ? red : blue;
    paint_object(fb, circle, color);
    usleep(200000);
  }
}

void *random_walk(void *args) {
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
  // struct object *square = create_square(x_start, y_start, 20);
  struct object *circle = create_circle(x_start, y_start, 20);

  paint_object(fb, circle, white);

  int min = -1;
  int max = 1;
  // int step_size = 20;
  int step_size = 10;

  while (1) {
    paint_object(fb, circle, black);

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

    shift_object(circle, x, y);

    uint16_t color =
        get_color(rand_range(0, 255), rand_range(0, 255), rand_range(0, 255));

    paint_object(fb, circle, color);
    usleep(200000);
  }
}
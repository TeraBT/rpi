#include <fcntl.h>
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

#define OBJECT_COUNT 5
object *objects[OBJECT_COUNT];
object *movable_object;
size_t max_dist = 0;

int rand_range(int min, int max) { return min + rand() % (max - min + 1); }

void *random_walk(void *args);
void *static_dist(void *args);
void *random_walk_dist(void *args);
void *gravity_fixpoint(void *args);
void *object_rotation(void *args);

void *global_routine(void *args);

int main(void) {
  enable_raw_mode();

  system("echo 0 | sudo tee /sys/class/graphics/fbcon/cursor_blink >/dev/null");
  system("tput clear | sudo tee /dev/tty1 > /dev/null");

  srand(time(NULL));

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
  object *square = create_circle(x_start, y_start, 20);
  movable_object = square;

  draw_object(fb, square, white);

  size_t thread_indices[OBJECT_COUNT];

  for (size_t i = 0; i < OBJECT_COUNT; i++) {
    objects[i] = create_circle(rand_range(800, 1200), rand_range(300, 700), 20);
  }

  for (size_t i = 0; i < OBJECT_COUNT; i++) {
    pthread_t t;
    thread_indices[i] = i;
    pthread_create(&t, NULL, object_rotation, &thread_indices[i]);
  }

  pthread_t t;
  pthread_create(&t, NULL, global_routine, NULL);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  while (1) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {

      if (c == 'q')
        break;
      else if (c == 'h') {
        draw_object(fb, square, black);
        shift_object(square, -5, 0);
      } else if (c == 'l') {
        draw_object(fb, square, black);
        shift_object(square, 5, 0);
      } else if (c == 'k') {
        draw_object(fb, square, black);
        shift_object(square, 0, -5);
      } else if (c == 'j') {
        draw_object(fb, square, black);
        shift_object(square, 0, 5);
      }
    }

    draw_object(fb, square, white);
  }

  free(square);
  close_fb(fb);

  disable_raw_mode();
}

void *global_routine(void *args) {
  while (1) {
    int distances[OBJECT_COUNT];
    int current_max_dist = 0;
    for (size_t i = 0; i < OBJECT_COUNT; i++) {
      distances[i] = compute_distance(movable_object, objects[i]);
      if (distances[i] > current_max_dist)
        current_max_dist = distances[i];
    }

    max_dist = current_max_dist;
  }
}

void *object_rotation(void *args) {
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
  object *rectangle =
      create_object(x_start, y_start, x_start + 100, y_start + 20);
  size_t thread_index = *(size_t *)args;
  objects[thread_index] = rectangle;

  double rotational_degrees = rand() % 2 == 0 ? 5 : -5;
  draw_object(fb, rectangle, white);
  while (1) {
    draw_object(fb, rectangle, black);
    rotate_object(rectangle, rotational_degrees);
    draw_object(fb, rectangle, white);
    usleep(50000);
  }
}

void *gravity_fixpoint(void *args) {
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
  object *circle = create_circle(x_start, y_start, 20);
  size_t thread_index = *(size_t *)args;
  objects[thread_index] = circle;

  draw_object(fb, circle, white);

  vector speed = {10., 0.};
  while (1) {

    object *trajectory_object =
        create_circle(circle->center.x, circle->center.y, 20);
    object *trajectory_artifact =
        create_square(circle->center.x, circle->center.y, 4);
    vector trajectory_speed = speed;

    for (size_t i = 0; i < 20; i++) {
      vector trajectory_pull =
          compute_gravitational_pull(trajectory_object, movable_object);
      trajectory_speed.x += trajectory_pull.x;
      trajectory_speed.y += trajectory_pull.y;
      shift_object(trajectory_object, trajectory_speed.x, trajectory_speed.y);
      shift_object(trajectory_artifact, trajectory_speed.x, trajectory_speed.y);
      draw_object(fb, trajectory_artifact, red);
    }

    size_t distance = compute_distance(circle, movable_object);
    vector pull = compute_gravitational_pull(circle, movable_object);
    speed.x += pull.x;
    speed.y += pull.y;

    draw_object(fb, circle, black);
    shift_object(circle, speed.x, speed.y);
    uint16_t color = compute_bgr_heatmap((double)distance / max_dist);
    draw_object(fb, circle, color);

    // printf("i = %ld. Dist. = %ld. Max. dist. = %ld. Rel. dist. = %f\n",
    //        thread_index, distance, max_dist, (double)distance / max_dist);

    // printf("Distance of object %ld to movable object is %ld.\n",
    // thread_index,
    //        distance);

    // vector directional_vector =
    //     compute_directional_vector(circle->center, movable_object->center);
    // printf("Directional vector of object %ld to movable object is (%f,
    // %f).\n",
    //        thread_index, directional_vector.x, directional_vector.y);

    // printf("Pull of object %ld to movable object is (%f, %f).\n",
    // thread_index,
    //        pull.x, pull.y);

    usleep(200000);
  }
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
  objects[thread_index] = circle;

  draw_object(fb, circle, white);

  int min = -1;
  int max = 1;
  int step_size = 10;

  while (1) {
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

    size_t distance = compute_distance(circle, movable_object);

    draw_object(fb, circle, black);
    shift_object(circle, x, y);
    uint16_t color = distance < 100 ? red : blue;
    draw_object(fb, circle, color);

    // printf("Distance of object %ld to movable object is %ld.\n",
    // thread_index,
    //        distance);

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
  objects[thread_index] = circle;

  draw_object(fb, circle, white);

  while (1) {
    draw_object(fb, circle, black);

    size_t distance = compute_distance(circle, movable_object);
    printf("Distance of object %ld to movable object is %ld.\n", thread_index,
           distance);

    uint16_t color = distance < 100 ? red : blue;
    draw_object(fb, circle, color);
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

  draw_object(fb, circle, white);

  int min = -1;
  int max = 1;
  // int step_size = 20;
  int step_size = 10;

  while (1) {
    draw_object(fb, circle, black);

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

    draw_object(fb, circle, color);
    usleep(200000);
  }
}
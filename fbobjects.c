#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "fblib.h"

static struct termios oldt;

void enable_raw_mode(void) {
  struct termios t;

  tcgetattr(STDIN_FILENO, &oldt);
  t = oldt;

  t.c_lflag &= ~ICANON;

  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void disable_raw_mode(void) { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); }

void paint_object(struct framebuffer *fb, int *object, size_t len,
                  uint16_t color) {
  for (size_t i = 0; i < len; i += 2) {
    paint_pixel(fb, object[i], object[i + 1], color);
  }
}

void shift_object(int *object, size_t len, int shift_x, int shift_y) {
  for (size_t i = 0; i < len; i += 2) {
    object[i] += shift_x;
  }

  for (size_t i = 1; i < len; i += 2) {
    object[i] += shift_y;
  }
}

int main(void) {
  enable_raw_mode();

  int line[] = {0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0};

  struct framebuffer framebuffer;
  struct framebuffer *fb = &framebuffer;
  open_fb(fb);

  size_t len = sizeof(line) / sizeof(line[0]);
  uint16_t white = get_color(255, 255, 255);
  uint16_t red = get_color(255, 0, 0);
  uint16_t black = get_color(0, 0, 0);

  paint_object(fb, line, len, white);

  for (size_t i = 0; i < 5; i++) {
    shift_object(line, len, 10, 10);
    paint_object(fb, line, len, red);
  }

  while (1) {
    int c = getchar();

    if (c == 'q')
      break;
    else if (c == 'h') {
      paint_object(fb, line, len, black);
      shift_object(line, len, -1, 0);
    } else if (c == 'l') {
      paint_object(fb, line, len, black);
      shift_object(line, len, 1, 0);
    } else if (c == 'k') {
      paint_object(fb, line, len, black);
      shift_object(line, len, 0, -1);
    } else if (c == 'j') {
      paint_object(fb, line, len, black);
      shift_object(line, len, 0, 1);
    }

    paint_object(fb, line, len, red);
  }

  disable_raw_mode();
}
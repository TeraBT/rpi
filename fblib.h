#ifndef FBLIB_H
#define FBLIB_H
#include <linux/fb.h>
#include <stdint.h>
#include <stdlib.h>

struct framebuffer {
  int fb_fd;
  uint16_t *fb_ptr;
  size_t height;
  size_t width;
  size_t line_length;
  size_t bits_per_pixel;
};

void open_fb(struct framebuffer *fb);

void close_fb(struct framebuffer *fb);

uint16_t get_color(size_t r, size_t g, size_t b);

void paint_screen(struct framebuffer *fb, uint16_t color);

void paint_pixel(struct framebuffer *fb, int x, int y, uint16_t color);

#endif
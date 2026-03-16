#include "fblib.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

void open_fb(struct framebuffer *fb) {
  int fb_fd = open("/dev/fb0", O_RDWR);

  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;

  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
  ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

  size_t height = vinfo.yres;
  size_t width = vinfo.xres;
  size_t line_length = finfo.line_length;

  uint16_t *fb_ptr = mmap(0, height * line_length, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fb_fd, 0);

  fb->fb_fd = fb_fd;
  fb->fb_ptr = fb_ptr;
  fb->height = height;
  fb->width = width;
  fb->line_length = line_length;
  fb->bits_per_pixel = vinfo.bits_per_pixel;
}

void close_fb(struct framebuffer *fb) {
  munmap(fb->fb_ptr, fb->height * fb->width);
  close(fb->fb_fd);
}

uint16_t get_color(size_t r, size_t g, size_t b) {
  return (size_t)(r / 100.0 * 31) << 11 | (size_t)(g / 100.0 * 63) << 5 |
         (size_t)(b / 100.0 * 31);
}

void paint_screen(struct framebuffer *fb, uint16_t color) {
  for (size_t y = 0; y < fb->height; y++)
    for (size_t x = 0; x < fb->width; x++) {

      size_t pos = y * fb->line_length + x * (fb->bits_per_pixel / 8);

      *(fb->fb_ptr + pos / sizeof(*fb->fb_ptr)) = color;
    }
}

void paint_pixel(struct framebuffer *fb, int x, int y, uint16_t color) {

  size_t pos = y * fb->width + x * (fb->bits_per_pixel / 8);
  *(fb->fb_ptr + pos / sizeof(*fb->fb_ptr)) = color;
}
#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

uint16_t get_color(size_t r, size_t g, size_t b) {
  return (size_t)(r / 100.0 * 31) << 11 | (size_t)(g / 100.0 * 63) << 5 |
         (size_t)(b / 100.0 * 31);
}

void paint_screen(uint16_t *fbp, struct fb_var_screeninfo *vinfo,
                  struct fb_fix_screeninfo *finfo, uint16_t color);

void paint_pixel(uint16_t *fbp, struct fb_var_screeninfo *vinfo,
                 struct fb_fix_screeninfo *finfo, int x, int y, uint16_t color);

void paint_grid_onto_screen(uint8_t *grid, uint16_t *fbp,
                            struct fb_var_screeninfo *vinfo,
                            struct fb_fix_screeninfo *finfo);

int main() {

  int fb = open("/dev/fb0", O_RDWR);

  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;

  ioctl(fb, FBIOGET_VSCREENINFO, &vinfo);
  ioctl(fb, FBIOGET_FSCREENINFO, &finfo);

  size_t screensize = vinfo.yres * finfo.line_length;

  uint16_t *fbp =
      mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);

  srand(time(NULL));

  uint8_t *grid = malloc(vinfo.yres * vinfo.xres * sizeof(uint8_t));
  uint8_t *next_grid = malloc(vinfo.yres * vinfo.xres * sizeof(uint8_t));

  for (size_t y = 0; y < vinfo.yres; y++)
    for (size_t x = 0; x < vinfo.xres; x++) {
      grid[y * vinfo.xres + x] = rand() % 2;
    }

  paint_grid_onto_screen(grid, fbp, &vinfo, &finfo);

  while (1) {
    for (long y = 0; y < vinfo.yres; y++)
      for (long x = 0; x < vinfo.xres; x++) {

        uint8_t living_neighbour_count = 0;
        for (int y_offset = -1; y_offset < 2; y_offset++)
          for (int x_offset = -1; x_offset < 2; x_offset++) {

            if (y_offset == 0 && x_offset == 0)
              continue;

            if ((y + y_offset) >= (long)vinfo.yres || (y + y_offset) < 0 ||
                (x + x_offset) >= (long)vinfo.xres || (x + x_offset) < 0)
              continue;

            long pos = (y + y_offset) * vinfo.xres + x + x_offset;

            living_neighbour_count += grid[pos];
          }

        size_t current_pos = y * vinfo.xres + x;
        int is_alive = grid[current_pos];

        if (is_alive)
          next_grid[current_pos] =
              living_neighbour_count == 2 || living_neighbour_count == 3;
        else
          next_grid[current_pos] = living_neighbour_count == 3;
      }

    paint_grid_onto_screen(next_grid, fbp, &vinfo, &finfo);
    uint8_t *tmp_ptr = grid;
    grid = next_grid;
    next_grid = tmp_ptr;
  }

  free(grid);
  free(next_grid);
  munmap(fbp, screensize);
  close(fb);
}

void paint_screen(uint16_t *fbp, struct fb_var_screeninfo *vinfo,
                  struct fb_fix_screeninfo *finfo, uint16_t color) {
  for (size_t y = 0; y < vinfo->yres; y++)
    for (size_t x = 0; x < vinfo->xres; x++) {

      size_t pos = y * finfo->line_length + x * (vinfo->bits_per_pixel / 8);

      *(fbp + pos / sizeof(*fbp)) = color;
    }
}

void paint_pixel(uint16_t *fbp, struct fb_var_screeninfo *vinfo,
                 struct fb_fix_screeninfo *finfo, int x, int y,
                 uint16_t color) {
  size_t pos = y * finfo->line_length + x * (vinfo->bits_per_pixel / 8);
  *(fbp + pos / sizeof(*fbp)) = color;
}

void paint_grid_onto_screen(uint8_t *grid, uint16_t *fbp,
                            struct fb_var_screeninfo *vinfo,
                            struct fb_fix_screeninfo *finfo) {
  for (size_t y = 0; y < vinfo->yres; y++)
    for (size_t x = 0; x < vinfo->xres; x++) {
      paint_pixel(fbp, vinfo, finfo, x, y,
                  grid[y * vinfo->xres + x] == 0 ? 0x0000 : 0xffff);
    }
}
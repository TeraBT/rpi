CC = aarch64-linux-gnu-gcc
CFLAGS = -O2 -mcpu=cortex-a53 -Werror -Wall -Wextra -Wpedantic

fbobjects: fbobjects.c fblib.c
	$(CC) $(CFLAGS) fbobjects.c fblib.c -o fbobjects

gol-framebuffer: gol-framebuffer.c
	$(CC) $(CFLAGS) gol-framebuffer.c -o gol-framebuffer

fblib: fblib.c
	$(CC) $(CFLAGS) -shared -fPIC -O2 fblib.c -o fblib.so

clean:
	rm fbobjects gol-framebuffer fblib.so

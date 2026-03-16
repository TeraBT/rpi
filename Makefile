CC = aarch64-linux-gnu-gcc
CFLAGS = -O2 -mcpu=cortex-a53 -Werror -Wall -Wextra -Wpedantic

all: gol-framebuffer.c
	$(CC) $(CFLAGS) gol-framebuffer.c -o gol-framebuffer

fblib: fblib.c
	$(CC) $(CFLAGS) -shared -fPIC -O2 fblib.c -o fblib.so

clean:
	rm gol-framebuffer fblib.so

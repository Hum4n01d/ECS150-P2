# Target library
lib := libuthread.a
objs    := queue.o

CC	:= gcc
CFLAGS  := -Wall -Wextra -Werror -MMD

all: $(lib)

$(lib): $(objs)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.d *.a
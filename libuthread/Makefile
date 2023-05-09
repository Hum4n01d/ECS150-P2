# Target library
lib 		:= libuthread.a
objs    := queue.o uthread.o context.o preempt.o sem.o

# removed -Werror for now

CC			:= gcc
CFLAGS  := -Wall -Wextra -MMD -g

all: $(lib)

$(lib): $(objs)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.d *.a
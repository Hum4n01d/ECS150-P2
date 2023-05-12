# Target library
lib 		:= libuthread.a
objs    := queue.o uthread.o context.o preempt.o sem.o

CC			:= gcc
CFLAGS  := -Wall -Wextra -Werror -MMD -g
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

all: $(lib)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

$(lib): $(objs)
	@echo "CC	$@"
	$(Q)ar rcs $@ $^

%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "CLEAN"
	$(Q)rm -f $(lib) $(objs) $(deps)
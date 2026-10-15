CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
TARGETS = parent child

all: $(TARGETS)

parent: parent.c
	$(CC) $(CFLAGS) -o $@ $<

child: child.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
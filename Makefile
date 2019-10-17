TARGETS = mkloadstring loadstring-driver-basis loadstring-driver

CC = gcc
CFLAGS = -Wall -Werror -g
LDFLAGS = $(CFLAGS)

all: $(TARGETS)

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

mkloadstring: mkloadstring.o
	$(CC) -o $@ $(LDFLAGS) $^

loadstring-driver-basis: loadstring-driver.c loadstring-basis.s
	$(CC) -o $@ $(LDFLAGS) $^

INTERMEDIATES += loadstring-basis.s
loadstring-basis.s: loadstring-basis.c
	$(CC) -o $@ $(CFLAGS) -S -c $<

loadstring-driver: loadstring-driver.c loadstring-shellcode.s
	$(CC) -o $@ $(LDFLAGS) $^

INTERMEDIATES += loadstring-shellcode.s
loadstring-shellcode.s: mkloadstring loadstring-template.s
	cat loadstring-template.s | awk -- '/SHELLCODE/ { exit } { print $0 }' > $@
	./mkloadstring -p 'Load me, man!' >> $@
	cat loadstring-template.s | awk -- '/SHELLCODE/ { found=1 } { if (found) print $0 }' >> $@

.PHONY: clean
clean:
	rm -f *.o *~ $(TARGETS) $(INTERMEDIATES)

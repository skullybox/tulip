# Top-level make file
C = clang

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

ldflgs :=
incflgs := -I.
cflgs := -Werror -D_FILE_OFFSET_BITS=64 -std=gnu11 -c

all: main

main: ${objects}
	${C} -o tulip ${objects}

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	-@rm *.o
	-@rm tulip

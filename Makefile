# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

ldflgs :=
incflgs := -I.
cflgs := -Werror -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -D_FILE_OFFSET_BITS=64 -std=gnu11 -c

all: main

main: ${objects}
	${C} -lpthread -o tulip ${objects}

win: ${objects}
		${C} -o tulip ${objects} -lpthread -lws2_32

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	-@rm *.o
	-@rm tulip

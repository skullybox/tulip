# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

ldflgs :=
incflgs := -I. -I./lib/libgcrypt-1.7.2/include
cflgs := -O3 -Werror -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -D_FILE_OFFSET_BITS=64 -std=gnu11 -c

all: crypt main

main: ${objects}
	${C} -lpthread -o tulip ${objects} ./lib/libgcrypt-1.7.2/lib/libgcrypt.a

win: ${objects}
		${C} -o tulip ${objects} ./lib/libgcrypt-1.7.2/lib/libgcrypt.a -lpthread -lws2_32

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	-@rm *.o
	-@rm tulip
	-@del *.o
	-@del tulip.exe

clean-all:
	-@rm *.o
	-@rm tulip
	-@del *.o
	-@del tulip.exe
	-@rm -rf lib/libgcrypt-1.7.2
	-@del /s /q lib/libgcrypt-1.7.2


crypt:
	if [ ! -d "lib/libgcrypt-1.7.2" ]; then \
		tar jxf "lib/libgcrypt-1.7.2.tar.bz2" -C "lib";\
		cd "lib/libgcrypt-1.7.2" && ./configure --disable-shared --prefix=`pwd` && ${MAKE} && ${MAKE} install;\
	fi

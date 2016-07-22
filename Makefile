# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

ldflgs := -L./lib/libgcrypt-1.7.2/lib/
incflgs := -I. -I./lib/libgcrypt-1.7.2/include
cflgs := -g -Werror -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -D_FILE_OFFSET_BITS=64 -std=gnu11 -c

all: crypt main

tags: 
	ctags -R --append=no .

main: ${objects}
	${C} -lpthread -lgcrypt ${ldflgs} -o tulip ${objects} ${incflgs}

win: crypt ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread -lws2_32 -lgcrypt -lgpg-error 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	-@rm *.o
	-@rm tulip

clean-all:
	-@rm *.o
	-@rm tulip
	-@rm -rf lib/libgcrypt-1.7.2


crypt:
	if [ ! -d "lib/libgcrypt-1.7.2" ]; then \
		tar jxf "lib/libgcrypt-1.7.2.tar.bz2" -C "lib";\
		cd "lib/libgcrypt-1.7.2" && ./configure --disable-shared --prefix=`pwd` && ${MAKE} && ${MAKE} install;\
	fi

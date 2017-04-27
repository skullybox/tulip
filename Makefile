# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

ldflgs := -L./lib/libgcrypt-1.7.2/lib/
incflgs := -I. -I./lib/libgcrypt-1.7.2/include
cflgs := -O3 -Werror -Wno-implicit-function-declaration -D_FILE_OFFSET_BITS=64 -std=gnu11 -c
#cflgs := -O0 -g -Werror -Wno-implicit-function-declaration -D_FILE_OFFSET_BITS=64 -std=gnu11 -c

all: crypt main

tags: 
	ctags -td *.h *.c

main: ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread -lgcrypt -lgpg-error

win: crypt ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread -lws2_32 -lgcrypt -lgpg-error 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	@-rm -f *.o || true
	@-rm -f tulip || true

clean-all:
	@-rm -f *.o || true
	@-rm -f tulip || true
	@-rm -rf lib/libgcrypt-1.7.2 || true


crypt:
	if [ ! -d "lib/libgcrypt-1.7.2" ]; then \
		tar jxf "lib/libgcrypt-1.7.2.tar.bz2" -C "lib";\
		cd "lib/libgcrypt-1.7.2" && ./configure --disable-shared --prefix=`pwd` && ${MAKE} && ${MAKE} install;\
	fi

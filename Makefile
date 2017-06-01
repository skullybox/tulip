# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

cflgs := -O3 -Werror -D_FILE_OFFSET_BITS=64 -std=gnu99 -c
#cflgs := -O0 -g -Werror -D_FILE_OFFSET_BITS=64 -std=c99 -c

.PHONY: tls
all: tls main

tags: 
	ctags -td *.h *.c

main: ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread 

win: crypt ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread -lws2_32 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	@-rm -f *.o || true
	@-rm -f tulip || true

clean-all:
	@-rm -f *.o || true
	@-rm -f tulip || true

tls:
	if [ ! -d "tls/mbedtls-2.4.2" ]; then \
		tar zxf "tls/mbedtls-2.4.2-apache.tgz" -C "tls";\
		cd "tls/mbedtls-2.4.2" && cmake . && ${MAKE};\
	fi


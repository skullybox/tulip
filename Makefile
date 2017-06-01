# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

inc := -I./tls/mbedtls-2.4.2/include
libs := -L./tls/mbedtls-2.4.2/library -lmbedcrypto -lmbedtls -lmbedx509
cflgs := -O3 -Werror -D_FILE_OFFSET_BITS=64 -std=gnu99 ${inc} -c

.PHONY: tls
all: tls main

tags: 
	ctags -td *.h *.c

main: ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread ${libs}

win: crypt ${objects}
	${C} ${ldflgs} -o tulip ${objects} ${incflgs} -lpthread -lws2_32 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean:
	@-rm -f *.o || true
	@-rm -f tulip || true
	@-rm -rf tls/mbedtls-2.4.2 || true

tls:
	if [ ! -d "tls/mbedtls-2.4.2" ]; then \
		tar zxf "tls/mbedtls-2.4.2-apache.tgz" -C "tls";\
		cd "tls/mbedtls-2.4.2" && cmake . && ${MAKE};\
	fi


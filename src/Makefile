# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})

inc := -I./tls/mbedtls-2.4.2/include
libs := -L./tls/mbedtls-2.4.2/library -lmbedtls -lmbedcrypto -lmbedx509
cflgs := -O3 -Werror -D_FILE_OFFSET_BITS=64 -std=gnu99 ${inc} -c

# we'll add more client names here
ifeq ($(CL),test)
  cflgs := -DCLIENT_TEST $(cflgs)
  certs := certs/test/server_cert.c certs/test/ca_cert.c certs/test/client_cert.c
endif

PREFIX := "rpmbuild/BUILDROOT/tulip-1.0-1.x86_64"

.PHONY: tls
all: tls main

tags: 
	ctags -td *.h *.c

main: ${objects}
	${C} ${ldflgs} ${certs} -o tulip ${objects} ${incflgs} -lpthread ${libs}

win: crypt ${objects}
	${C} ${ldflgs} ${certs} -o tulip ${objects} ${incflgs} -lpthread -lws2_32 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o $@

clean-all:
	@-rm -f *.o || true
	@-rm -f tulip || true
	@-rm -rf tls/mbedtls-2.4.2 || true

install:
	-mkdir -p "../${PREFIX}/etc/init.d/"
	-mkdir -p "../${PREFIX}/etc/tulip"
	cp "tulip" "../${PREFIX}/etc/tulip/tulip"
	cp "config.cfg" "../${PREFIX}/etc/tulip/config.cfg"

clean:
	@-rm -f *.o || true
	@-rm -f tulip || true

tls:
	if [ ! -d "tls/mbedtls-2.4.2" ]; then \
		tar zxf "tls/mbedtls-2.4.2-apache.tgz" -C "tls";\
		cd "tls/mbedtls-2.4.2" && cmake . && ${MAKE};\
	fi


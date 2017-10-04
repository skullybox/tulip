# Top-level make file
C = gcc

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})
objects_bin := $(patsubst %.c, .obj/%.o, ${sources})

inc := -I./tls/mbedtls-2.4.2/include -Isqlite
libs := -L./tls/mbedtls-2.4.2/library -lmbedtls -lmbedcrypto -lmbedx509 
cflgs := -DSYSLOG_USE=1 -O3 -Werror -D_FILE_OFFSET_BITS=64 -std=gnu99 ${inc} -c

# we'll add more client names here
ifeq ($(CL),test)
  cflgs := -DCLIENT_TEST $(cflgs)
  certs := certs/test/server_cert.c certs/test/ca_cert.c certs/test/client_cert.c certs/test/com_key.c
endif

PREFIX := "rpmbuild/BUILDROOT/tulip-1.0-1.x86_64"

.PHONY: tls
all: prep tls sql main
mysql: prep tls mysqlc with_mysql

tags: 
	ctags -R --c-kinds=+ --fields=+iaS --extra=+q .
	ctags -R --c-kinds=+ -a --fields=+iaS --extra=+q /usr/include

prep: 
	@-mkdir .obj

main: ${objects}
	${C} ${ldflgs} ${certs} -o tulip ${objects_bin} .obj/sqlite3.o ${inc} -lpthread ${libs} -ldl

with_mysql: ${objects}
	${C} ${ldflgs} ${certs} -o tulip ${objects_bin} .obj/sqlite3.o ${inc} -lpthread ${libs} -ldl

win: crypt ${objects}
	${C} ${ldflgs} ${certs} -o tulip ${objects_bin} ${inc} -lpthread -lws2_32 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o .obj/$@

clean-all:
	@-rm -rf .obj || true
	@-rm -f tulip || true
	@-rm -rf tls/mbedtls-2.4.2 || true
	@-rm -rf mysql/mysql-connector-c-6.1.10-src || true

install:
	-mkdir -p "../${PREFIX}/etc/init.d/"
	-mkdir -p "../${PREFIX}/etc/tulip"
	cp "tulip" "../${PREFIX}/etc/tulip/tulip"

clean:
	@-rm -rf .obj || true
	@-rm -f tulip || true

tls:
	if [ ! -d "tls/mbedtls-2.4.2" ]; then \
		tar zxf "tls/mbedtls-2.4.2-apache.tgz" -C "tls";\
		cd "tls/mbedtls-2.4.2" && cmake . && ${MAKE};\
	fi

sql:
	gcc ${cflgs} -DSQLITE_THREADSAFE=2 -Isqlite sqlite/sqlite3.c -o .obj/sqlite3.o

mysqlc:
		if [ ! -d "mysql/mysql-connector-c-6.1.10-src" ]; then \
			tar zxf "mysql/mysql-connector-c-6.1.10-src.tar.gz" -C "mysql";\
			cd "mysql/mysql-connector-c-6.1.10-src" && cmake . && ${MAKE};\
		fi

# Top-level make file
C = gcc

ANDROID_CC_TOOLS = "$(HOME)/Android/Sdk/ndk-bundle/build/tools/make-standalone-toolchain.sh"
ANDROID_CC = "arm/bin/arm-linux-androideabi-gcc"
ANDROID_CPP = "arm/bin/arm-linux-androideabi-g++"

sources := $(wildcard *.c)
objects := $(patsubst %.c, %.o, ${sources})
objects_bin := $(patsubst %.c, .obj/%.o, ${sources})

inc := -I./tls/mbedtls-2.4.2/include -Isqlite
libs := -L./tls/mbedtls-2.4.2/library -lmbedtls -lmbedcrypto -lmbedx509 
cflgs := -DSYSLOG_USE -O3 -Werror -D_FILE_OFFSET_BITS=64 -std=gnu99 ${inc} -c

ifeq ($(USE_MYSQL),1)
  cflgs := -DUSE_MYSQL $(cflgs)
	my_inc := -I./mysql/mysql-connector-c-6.1.10-src/include 
	my_lib := -L./mysql/mysql-connector-c-6.1.10-src/libmysql -lmysqlclient
endif

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
	@-mkdir out
	@-mkdir .obj

main: ${objects}
	${C} ${ldflgs} ${certs} -o out/tulip ${objects_bin} .obj/sqlite3.o ${inc} -lpthread ${libs} -ldl

with_mysql: ${objects}
	${C} ${ldflgs} ${certs} -o out/tulip ${objects_bin} ${inc} ${my_inc} -lpthread ${libs} -ldl ${my_lib}

win: crypt ${objects}
	${C} ${ldflgs} ${certs} -o out/tulip ${objects_bin} ${inc} -lpthread -lws2_32 

%.o : %.c
	${C} ${cflgs} ${incflgs} $^ -o .obj/$@

clean-all: clean
	@-rm -rf tls/mbedtls-2.4.2 || true
	@-rm -rf mysql/mysql-connector-c-6.1.10-src || true

install:
	-mkdir -p "../${PREFIX}/etc/init.d/"
	-mkdir -p "../${PREFIX}/etc/tulip"
	cp "out/tulip" "../${PREFIX}/etc/tulip/tulip"

clean:
	@-rm -rf .obj || true
	@-rm -f tulip || true
	@-rm -f tulip.db || true

tls:
	if [ ! -d "tls/mbedtls-2.4.2" ]; then \
		tar zxf "tls/mbedtls-2.4.2-apache.tgz" -C "tls";\
		cd "tls/mbedtls-2.4.2" && cmake -DCMAKE_BUILD_TYPE=Release . && ${MAKE};\
	fi

sql:
	gcc ${cflgs} -DSQLITE_THREADSAFE=2 -Isqlite sqlite/sqlite3.c -o .obj/sqlite3.o

mysqlc:
	if [ ! -d "mysql/mysql-connector-c-6.1.10-src" ]; then \
	tar zxf "mysql/mysql-connector-c-6.1.10-src.tar.gz" -C "mysql";\
	cd "mysql/mysql-connector-c-6.1.10-src" && cmake . && ${MAKE};\
	fi

android_setup: 
	@-rm -rf arm || true
	if [ ! -d arm ]; then \
		${ANDROID_CC_TOOLS}  --platform=android-19 --abis=${android_abi} --install-dir=arm;\
		fi

tls_android: android_setup
	@-rm -rf tls/mbedtls-2.4.2 || true
	if [ ! -d "tls/mbedtls-2.4.2" ]; then \
		tar zxf "tls/mbedtls-2.4.2-apache.tgz" -C "tls";\
		cd "tls/mbedtls-2.4.2" && CC=$(HOME)/${ANDROID_CC} CXX=$(HOME)/${ANDROID_CPP} cmake -DCMAKE_BUILD_TYPE=Release -DUSE_SHARED_MBEDTLS_LIBRARY=On . && ${MAKE} lib;\
	fi


android: clean-all prep android_setup tls_android
	@-mkdir -p out/${android_abi}
	cp tls/mbedtls-2.4.2/library/libmbedcrypto.so.2.4.2 ./out/${android_abi}/libmbedcrypto.so
	cp tls/mbedtls-2.4.2/library/libmbedtls.so.2.4.2 ./out/${android_abi}/libmbedtls.so
	cp tls/mbedtls-2.4.2/library/libmbedx509.so.2.4.2 ./out/${android_abi}/libmbedx509.so
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_userc.c -o out/${android_abi}/tul_userc.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_tls_common.c -o out/${android_abi}/tul_tls_common.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_tls_client.c -o out/${android_abi}/tul_tls_client.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_module_common.c -o out/${android_abi}/tul_module_common.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c whirlpool_hash.c -o out/${android_abi}/whirlpool_hash.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_random.c -o out/${android_abi}/tul_random.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c rc5_cipher.c -o out/${android_abi}/rc5_cipher.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_b64.c -o out/${android_abi}/tul_b64.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c tul_tcp_soc.c -o out/${android_abi}/tul_tcp_soc.o
	${ANDROID_CC} -fPIC -std=gnu99 ${inc} ${my_inc} -c certs/$(CL)/ca_cert.c -o out/${android_abi}/ca_cert.o
	${ANDROID_CC} -shared ${android_abi} -fPIC -Wl,-soname,tulipc.so -o ./out/${android_abi}/libtulipc.so out/${android_abi}/tul_userc.o out/${android_abi}/tul_tls_common.o out/${android_abi}/tul_tls_client.o out/${android_abi}/tul_module_common.o out/${android_abi}/whirlpool_hash.o out/${android_abi}/tul_random.o out/${android_abi}/rc5_cipher.o out/${android_abi}/tul_b64.o out/${android_abi}/tul_tcp_soc.o out/${android_abi}/ca_cert.o





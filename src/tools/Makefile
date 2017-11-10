# Top-level make file
C = gcc

inc := -I../tls/mbedtls-2.4.2/include -I../mysql/mysql-connector-c-6.1.10-src/include -I../
libs := -L../tls/mbedtls-2.4.2/library -lmbedtls -lmbedcrypto -lmbedx509 -ldl -lpthread -L../mysql/mysql-connector-c-6.1.10-src/libmysql -lmysqlclient
cflgs := -DSYSLOG_USE -O3 -Werror -D_FILE_OFFSET_BITS=64 -std=gnu99 ${inc} 

deps := ../tul_user.c ../tul_random.c ../tul_b64.c ../tul_log.c ../tul_mysql.c ../rc5_cipher.c

all: add_user

add_user: sql
	${C} ${cflgs} t_adduser.c ${deps} -o t_adduser ${libs}

sql:
	if [ ! -d "../mysql/mysql-connector-c-6.1.10-src" ]; then \
		tar zxf "../mysql/mysql-connector-c-6.1.10-src.tar.gz" -C "../mysql";\
		cd "../mysql/mysql-connector-c-6.1.10-src" && cmake . && ${MAKE};\
	fi



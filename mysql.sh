#!/bin/bash
set -eo pipefail
shopt -s nullglob

DATADIR="/var/lib/mysql"

if [ ! -d "$DATADIR/mysql" ];
then
  echo "Initialize mysql server..."
  mysqld --initialize-insecure --datadir="$DATADIR"
  mysql_ssl_rsa_setup --datadir="$DATADIR"
  chown -R mysql:mysql /var/lib/mysql

  echo "Start mysql in the background..."
  sudo -u mysql mysqld --skip-networking --datadir="$DATADIR" &
  mysql=( mysql --protocol=socket -uroot -hlocalhost --socket="/var/lib/mysql/mysql.sock" )

  echo "Configure root password on mysql..."
  sleep 40;
  "${mysql[@]}" -e "delete from mysql.user; create user 'root'@'%' identified by 'password'; \
  GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' WITH GRANT OPTION; \
  create user 'repl'@'%' identified by 'password'; GRANT REPLICATION SLAVE,REPLICATION CLIENT ON *.* TO 'repl'@'%';FLUSH PRIVILEGES;shutdown;"
else
  sudo -umysql mysqld --datadir="$DATADIR"

fi



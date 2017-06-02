
FROM ubuntu:16.04
ENV DEBIAN_FRONTEND noninteractive

# ------------------------
# /etc/mysql/my.cnf
# ------------------------
# bind-address = "0.0.0.0"
# port = 3306
# log-bin = mysql-bin
# server-id = <Server ID>
# relay-log = relay-log-slave
# enforce-gtid-consistency = true
# gtid-mode =ON
# datadir=/var/lib/mysql/
# socket=/var/lib/mysql/mysql.sock
# default_storage_engine = innodb
# binlog_format = row
# innodb_autoinc_lock_mode = 2
# innodb_flush_log_at_trx_commit = 0
# query_cache_size = 0
# query_cache_type = 0
# master-info-repository = TABLE
# ------------------------

RUN apt update 
RUN apt-get install -y wget
RUN wget https://dev.mysql.com/get/mysql-apt-config_0.8.6-1_all.deb
RUN dpkg -i *.deb
RUN rm *.deb
RUN apt-get update
RUN apt-get install -y mysql-server mysql-utilities openssl sudo

RUN if [ -f "/etc/mysql/my.cnf" ];then rm "/etc/mysql/my.cnf"; fi;
COPY my.cnf /etc/mysql/my.cnf
COPY mysql.sh /usr/local/bin/mysql.sh
RUN chmod 755 /usr/local/bin/mysql.sh
RUN rm -rf /var/lib/mysql/*
RUN /usr/local/bin/mysql.sh 
RUN rm "/usr/local/bin/mysql.sh"
EXPOSE 3306

ENTRYPOINT ["sudo", "-umysql","mysqld"]





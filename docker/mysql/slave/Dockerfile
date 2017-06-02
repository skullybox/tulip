
# build with the following parameters
# docker build -f Dockerfile-master  -t financeit:master .


FROM centos:centos7
MAINTAINER irfan 

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

# mysqlreplicate --slave=root:password@172.17.0.1:3307  --master=root:password@172.17.0.1:3306 --rpl-user=repl -vv --pedantic
# change master to master_host='172.17.0.1', master_user='root', master_password='password', master_auto_position=1;
RUN rpm -Uvh https://dev.mysql.com/get/mysql57-community-release-el7-11.noarch.rpm
RUN yum -y update
RUN yum -y install mysql-server mysql-utilities openssl sudo

RUN if [ -f "/etc/mysql/my.cnf" ];then rm "/etc/mysql/my.cnf"; fi;
COPY my.cnf /etc/mysql/my.cnf
COPY mysql.sh /usr/local/bin/mysql.sh
RUN chmod 755 /usr/local/bin/mysql.sh
RUN rm -rf /var/lib/mysql/*
EXPOSE 3307

ENTRYPOINT ["/usr/local/bin/mysql.sh"]





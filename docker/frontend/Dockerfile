FROM centos:centos7
MAINTAINER terry 

#RUN yum -y update
RUN yum -y install httpd mysql-utilities openssl sudo php mod_ssl php-mysql
RUN rm -f /var/www/html/index.*
COPY entrypoint.sh / 
COPY html/* /var/www/html/

ENTRYPOINT ["/entrypoint.sh"]
#ENTRYPOINT ["/usr/sbin/httpd -DFOREGROUND"]

# Start the service
#CMD ["-D", "FOREGROUND"]
#ENTRYPOINT ["/usr/sbin/httpd"]

EXPOSE 443

FROM centos:centos7
MAINTAINER terry 

#RUN yum -y update
RUN yum -y install httpd mysql-utilities openssl sudo php mod_ssl
COPY entrypoint.sh / 

#ENTRYPOINT ["/entrypoint.sh"]
#ENTRYPOINT ["/usr/sbin/httpd -DFOREGROUND"]

# Start the service
CMD ["-D", "FOREGROUND"]
ENTRYPOINT ["/usr/sbin/httpd"]

EXPOSE 443

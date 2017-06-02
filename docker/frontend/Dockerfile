FROM centos:centos7
MAINTAINER terry 

<<<<<<< HEAD
#RUN yum -y update
RUN yum -y install httpd mysql-utilities openssl sudo php mod_ssl
COPY entrypoint.sh / 

#ENTRYPOINT ["/entrypoint.sh"]
#ENTRYPOINT ["/usr/sbin/httpd -DFOREGROUND"]

# Start the service
CMD ["-D", "FOREGROUND"]
ENTRYPOINT ["/usr/sbin/httpd"]
=======
RUN yum -y update
RUN yum -y install httpd mysql-utilities openssl sudo php mod_ssl
>>>>>>> a57aa4bd17e06951285c99d80713380b01a502c2

EXPOSE 443

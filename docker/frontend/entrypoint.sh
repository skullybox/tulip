#!/bin/sh

set -e

#echo "Starting the mysql daemon"
#service mysql start

#echo "navigating to volume /var/www"
#cd /var/www
#echo "Creating soft link"
#ln -s /opt/mysite mysite

#a2enmod headers
#service httpd restart

#a2ensite mysite.conf
#a2dissite 000-default.conf
#service httpd reload

if [ -z "$1" ]
then
    /usr/sbin/httpd
#   service httpd start
#   systemctl start httpd

else
    exec "$1"
fi

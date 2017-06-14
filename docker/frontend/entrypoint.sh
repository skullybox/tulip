#!/bin/sh

set -e

echo "loading tulip.sql into database"

mysql -h percona -u tulip -ptulip cloud_auth < /tulip.sql

if [ -z "$1" ]
then
    /usr/sbin/httpd
    tail -fn300 /var/log/httpd/ssl_*
#   service httpd start
#   systemctl start httpd

else
    exec "$1"
fi

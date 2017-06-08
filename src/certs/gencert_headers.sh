#! /bin/bash

SERVER_PASS=$( cat "$1/pass.txt" | tr -s " " | grep "server private" | cut -f2 -d":" | sed 's/^ *//g')

CA_FILE="$1/demoCA/cacert.pem"
SERVER_KEY_FILE="$1/$(ls "$1/" | grep pem | grep -v csr)"
SERVER_CERT="$1/demoCA/newcerts/01.pem"

echo "const char CA_FILE [] = " > "$1/ca_cert.h"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/ca_cert.h"
done <  "$CA_FILE"
echo ";" >> "$1/ca_cert.h"


echo "const char SERVER_KEY [] = " > "$1/server_cert.h"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/server_cert.h"
done <  "$SERVER_KEY_FILE"
echo ";">> "$1/server_cert.h"

echo "const char SERVER_CERT [] = " >> "$1/server_cert.h"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/server_cert.h"
done <  "$SERVER_CERT"
echo ";">> "$1/server_cert.h"

echo "const char SERVER_KEY_PASS[] = \"$SERVER_PASS\";" >> "$1/server_cert.h"



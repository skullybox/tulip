#! /bin/bash

CA_CERT="$1/demoCA/cacert.pem"
SERVER_KEY_FILE="$1/$(ls "$1/" | grep pem | grep -v csr)"
SERVER_CERT="$1/demoCA/newcerts/01.pem"

echo "const char CA_CERT [] = " > "$1/ca_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/ca_cert.c"
done <  "$CA_CERT"
echo ";" >> "$1/ca_cert.c"

echo "const char SERVER_KEY [] = " > "$1/server_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/server_cert.c"
done <  "$SERVER_KEY_FILE"
echo ";">> "$1/server_cert.c"

echo "const char SERVER_CERT [] = " >> "$1/server_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/server_cert.c"
done <  "$SERVER_CERT"
echo ";">> "$1/server_cert.c"




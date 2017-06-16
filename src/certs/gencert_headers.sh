#! /bin/bash

COM_KEY=$(pwgen -s -1 16)
CA_CERT="$1/demoCA/cacert.pem"
SERVER_KEY_FILE="$1/$(ls "$1/" | grep pem | grep -v client | grep -v csr)"
SERVER_CERT="$1/demoCA/newcerts/01.pem"
CLIENT_CERT="$1/demoCA/newcerts/02.pem"
CLIENT_KEY_FILE="$1/$(ls "$1/" | grep pem | grep client | grep -v csr)"

echo "const char COM_KEY[]  = \"$COM_KEY\";" > "$1/com_key.c"

echo "const char CA_CERT [] = " > "$1/ca_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/ca_cert.c"
done <  "$CA_CERT"
echo ";" >> "$1/ca_cert.c"
echo "int CA_CERT_len = sizeof(CA_CERT);" >> "$1/ca_cert.c"

echo "const char SERVER_KEY [] = " > "$1/server_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/server_cert.c"
done <  "$SERVER_KEY_FILE"
echo ";">> "$1/server_cert.c"
echo "int SERVER_KEY_len = sizeof(SERVER_KEY);" >> "$1/server_cert.c"

echo "const char SERVER_CERT [] = " >> "$1/server_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/server_cert.c"
done <  "$SERVER_CERT"
echo ";">> "$1/server_cert.c"

echo "int SERVER_CERT_len = sizeof(SERVER_CERT);" >> "$1/server_cert.c"

echo "const char CLIENT_CERT [] = " > "$1/client_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/client_cert.c"
done <  "$CLIENT_CERT"
echo ";">> "$1/client_cert.c"
echo "int CLIENT_CERT_len = sizeof(CLIENT_CERT);" >> "$1/client_cert.c"

echo "const char CLIENT_KEY [] = " >> "$1/client_cert.c"
while read line;
do
 echo "\"$line\\r\\n\"" >> "$1/client_cert.c"
done <  "$CLIENT_KEY_FILE"
echo ";">> "$1/client_cert.c"
echo "int CLIENT_KEY_len = sizeof(CLIENT_KEY);" >> "$1/client_cert.c"





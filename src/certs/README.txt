Certificates here and their respective roots
are placed in respective folders here


- testing and development
    tests/ - are for testing and development
    tests/pass.txt - private key password for root CA, server cert


- generate root ca key
    openssl genrsa -aes256 -out ca.key.pem 4096

- generate root ca cert
    openssl req -outform pem -key ca.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out ca.cert.pem

- generate server private key
    openssl genrsa -aes256 -out test.thincatlabs.com.pem 2048

- generate server certificate signing 
    openssl req -key test.thincatlabs.com.pem -new -sha256 -out test.thincatlabs.csr.pem

- sign the certificate request (csr)
    openssl ca -extensions server_cert -days 375 -notext -md sha256 -in test.thincatlabs.csr.pem -out test.thincatlabs.com.cert.pem -CA ca.cert.pem -CAkey ca.key.pem




#!/bash/bin
#
# This script generates test certificates and CRLs for the poststarttests.
# 

#
# Set variables
#
CERT_NAME1=testdn1
CERT_NAME2=testdn2
CERT_NAME3=testdn3
CA_NAME1=testca1
OPENSSL_CNF=openssl.cnf
#OPENSSL_CNF=$PEGASUS_HOME/src/Server/ssl.cnf
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENSSL_HOME/lib

#
# Remove existing certs
#
rm -f $CERT_NAME1.cert
rm -f $CERT_NAME1.key
rm -f $CERT_NAME2.cert
rm -f $CERT_NAME2.key
rm -f $CERT_NAME3.cert
rm -f $CERT_NAME3.key
rm -f $CA_NAME1.cert
rm -f $CA_NAME1.key
rm -f $CA_NAME1.srl
rm -f index.txt

#
# Create a self-signed certificate
#
$OPENSSL_HOME/bin/openssl genrsa -out $CERT_NAME1.key 1024 
$OPENSSL_HOME/bin/openssl req -config $OPENSSL_CNF -new -key $CERT_NAME1.key -out $CERT_NAME1.csr < $CERT_NAME1.txt 
$OPENSSL_HOME/bin/openssl x509 -in $CERT_NAME1.csr -out $CERT_NAME1.cert -req -signkey $CERT_NAME1.key -days 356 
rm $CERT_NAME1.csr

#
# Create a self-signed CA 
#
$OPENSSL_HOME/bin/openssl genrsa -out $CA_NAME1.key 1024 
$OPENSSL_HOME/bin/openssl req -new -key $CA_NAME1.key -x509 -config $OPENSSL_CNF -days 365 -out $CA_NAME1.cert < $CA_NAME1.txt 

#
# Create a certificate signed by the CA
#
$OPENSSL_HOME/bin/openssl genrsa -out $CERT_NAME2.key 1024 
$OPENSSL_HOME/bin/openssl req -config $OPENSSL_CNF -new -key $CERT_NAME2.key -out $CERT_NAME2.csr < $CERT_NAME2.txt 
$OPENSSL_HOME/bin/openssl x509 -req -days 365 -in $CERT_NAME2.csr -CA $CA_NAME1.cert -CAkey $CA_NAME1.key -CAcreateserial -out $CERT_NAME2.cert 
rm $CERT_NAME2.csr

#
# Create another test certificate signed by the CA
#
$OPENSSL_HOME/bin/openssl genrsa -out $CERT_NAME3.key 1024 
$OPENSSL_HOME/bin/openssl req -config $OPENSSL_CNF -new -key $CERT_NAME3.key -out $CERT_NAME3.csr < $CERT_NAME3.txt 
$OPENSSL_HOME/bin/openssl x509 -req -days 365 -in $CERT_NAME3.csr -CA $CA_NAME1.cert -CAkey $CA_NAME1.key -CAcreateserial -out $CERT_NAME3.cert 
rm $CERT_NAME3.csr

#
# Create a self-signed CRL
#

# 
# Create a CRL issued by the test CA
#
touch index.txt
$OPENSSL_HOME/bin/openssl ca -config $OPENSSL_CNF -gencrl -keyfile $CA_NAME1.key -cert $CA_NAME1.cert -out $CA_NAME1.crl
$OPENSSL_HOME/bin/openssl ca -config $OPENSSL_CNF -revoke $CERT_NAME2.cert -keyfile $CA_NAME1.key -cert $CA_NAME1.cert 
$OPENSSL_HOME/bin/openssl ca -config $OPENSSL_CNF -gencrl -keyfile $CA_NAME1.key -cert $CA_NAME1.cert -out $CA_NAME1.crl 


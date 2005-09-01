#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
ROOT = ../../../..

DIR = Clients/ssltrustmgr/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak
include $(ROOT)/mak/commands.mak

RESULTFILE =  $(TMP_DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master
OPENSSL_CNF = openssl.cnf

tests:

poststarttests:

clean:
	@$(RM) $(RESULTFILE)
	@$(RM) $(PEGASUS_HOME)/testdn1.cert
	@$(RM) $(PEGASUS_HOME)/testdn1.key
	@$(RM) $(PEGASUS_HOME)/testdn2.cert
	@$(RM) $(PEGASUS_HOME)/testdn2.key
	@$(RM) $(PEGASUS_HOME)/testdn3.cert
	@$(RM) $(PEGASUS_HOME)/testdn3.key
	@$(RM) $(PEGASUS_HOME)/testdn4.cert
	@$(RM) $(PEGASUS_HOME)/testdn4.key
	@$(RM) $(PEGASUS_HOME)/testca1.cert
	@$(RM) $(PEGASUS_HOME)/testca1.key
	@$(RM) $(PEGASUS_HOME)/testca1.srl
	@$(RM) $(PEGASUS_HOME)/testca1.crl
	@$(RM) index.txt
	@$(RM) index.txt.attr
	@$(RM) index.txt.old
	@$(RM) .rnd

#
# Create certificates and CRLs required for running the tests
#
test_setup:
ifdef PEGASUS_HAS_SSL
	@$(MAKE) -i -s clean
	@#
	@# Create a self-signed certificate
	@#
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testdn1.key 1024
	@$(OPENSSL_COMMAND) req -config $(OPENSSL_CNF) -new -key $(PEGASUS_HOME)/testdn1.key -out $(PEGASUS_HOME)/testdn1.csr < testdn1.txt
	@$(OPENSSL_COMMAND) x509 -in $(PEGASUS_HOME)/testdn1.csr -out $(PEGASUS_HOME)/testdn1.cert -req -signkey $(PEGASUS_HOME)/testdn1.key -days 356
	@$(RM) $(PEGASUS_HOME)/testdn1.csr
	@#
	@# Create a self-signed CA
	@#
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testca1.key 1024
	@$(OPENSSL_COMMAND) req -new -key $(PEGASUS_HOME)/testca1.key -x509 -config $(OPENSSL_CNF) -days 365 -out $(PEGASUS_HOME)/testca1.cert < testca1.txt
	@#
	@# Create a certificate signed by the CA
	@#
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testdn2.key 1024
	@$(OPENSSL_COMMAND) req -config $(OPENSSL_CNF) -new -key $(PEGASUS_HOME)/testdn2.key -out $(PEGASUS_HOME)/testdn2.csr < testdn2.txt
	@$(OPENSSL_COMMAND) x509 -req -days 365 -in $(PEGASUS_HOME)/testdn2.csr -CA $(PEGASUS_HOME)/testca1.cert -CAkey $(PEGASUS_HOME)/testca1.key -CAcreateserial -out $(PEGASUS_HOME)/testdn2.cert
	@$(RM) $(PEGASUS_HOME)/testdn2.csr
	@#
	@# Create a second test certificate signed by the CA
	@#
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testdn3.key 1024
	@$(OPENSSL_COMMAND) req -config $(OPENSSL_CNF) -new -key $(PEGASUS_HOME)/testdn3.key -out $(PEGASUS_HOME)/testdn3.csr < testdn3.txt
	@$(OPENSSL_COMMAND) x509 -req -days 365 -in $(PEGASUS_HOME)/testdn3.csr -CA $(PEGASUS_HOME)/testca1.cert -CAkey $(PEGASUS_HOME)/testca1.key -CAcreateserial -out $(PEGASUS_HOME)/testdn3.cert
	@$(RM) $(PEGASUS_HOME)/testdn3.csr
	@#
	@# Create a third test certificate signed by the CA
	@#
	@$(OPENSSL_COMMAND) genrsa -out $(PEGASUS_HOME)/testdn4.key 1024
	@$(OPENSSL_COMMAND) req -config $(OPENSSL_CNF) -new -key $(PEGASUS_HOME)/testdn4.key -out $(PEGASUS_HOME)/testdn4.csr < testdn4.txt
	@$(OPENSSL_COMMAND) x509 -req -days 365 -in $(PEGASUS_HOME)/testdn4.csr -CA $(PEGASUS_HOME)/testca1.cert -CAkey $(PEGASUS_HOME)/testca1.key -CAcreateserial -out $(PEGASUS_HOME)/testdn4.cert
	@$(RM) $(PEGASUS_HOME)/testdn4.csr
	@#
	@# Create a CRL issued by the test CA and revoke a certificate
	@#
	@$(TOUCH) index.txt
	@$(OPENSSL_COMMAND) ca -config $(OPENSSL_CNF) -gencrl -keyfile $(PEGASUS_HOME)/testca1.key -cert $(PEGASUS_HOME)/testca1.cert -out $(PEGASUS_HOME)/testca1.crl
	@$(OPENSSL_COMMAND) ca -config $(OPENSSL_CNF) -revoke $(PEGASUS_HOME)/testdn2.cert -keyfile $(PEGASUS_HOME)/testca1.key -cert $(PEGASUS_HOME)/testca1.cert
	@$(OPENSSL_COMMAND) ca -config $(OPENSSL_CNF) -gencrl -keyfile $(PEGASUS_HOME)/testca1.key -cert $(PEGASUS_HOME)/testca1.cert -out $(PEGASUS_HOME)/testca1.crl
	@$(RM) index.txt.old
	@#
	@# Create a second CRL issued by the test CA and revoke two certificates
	@#
	@$(TOUCH) index.txt
	@$(OPENSSL_COMMAND) ca -config $(OPENSSL_CNF) -gencrl -keyfile $(PEGASUS_HOME)/testca1.key -cert $(PEGASUS_HOME)/testca1.cert -out $(PEGASUS_HOME)/testca2.crl
	@$(OPENSSL_COMMAND) ca -config $(OPENSSL_CNF) -revoke $(PEGASUS_HOME)/testdn4.cert -keyfile $(PEGASUS_HOME)/testca1.key -cert $(PEGASUS_HOME)/testca1.cert
	@$(OPENSSL_COMMAND) ca -config $(OPENSSL_CNF) -gencrl -keyfile $(PEGASUS_HOME)/testca1.key -cert $(PEGASUS_HOME)/testca1.cert -out $(PEGASUS_HOME)/testca2.crl
endif


SSLCertificateTest1:
ifdef PEGASUS_HAS_SSL
	@$(RM) $(RESULTFILE)
	@$(MAKE) -i -s SSLCertificateTest1_ignoreerror
	@$(STRIPCRS)
	$(COMPARERESULTS)
	@$(ECHO) +++ Test passed +++
	#@$(RM) $(RESULTFILE) Leaving this file here for now, so we have it to debug any poststarttest failures
endif


SSLCertificateTest1_ignoreerror:
ifdef PEGASUS_HAS_SSL
	@$(ECHO) ++++ssltrustmgr tests
ifndef PEGASUS_PAM_AUTHENTICATION
    ifneq ($(OS), zos)
	cimuser -a -u $(CURRENT_USER) -w $(CURRENT_USER)
    endif
endif	
	@$(ECHO) "Connect with an untrusted certificate -- should get a 401" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn1.cert $(PEGASUS_HOME)/testdn1.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the self-signed certificate to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testdn1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn1.cert $(PEGASUS_HOME)/testdn1.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Attempt to add the same certificate, this should get a duplication error" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testdn1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Delete the self-signed certificate from the truststore" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=TestSelfSigned1 -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect, this should fail and get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn1.cert $(PEGASUS_HOME)/testdn1.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Attempt to delete the certificate we just deleted, this should get a DNE error" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=TestSelfSigned1 -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the CA certificate to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testca1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the CA CRL to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -R -f $(PEGASUS_HOME)/testca1.crl >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate, this should fail with a connection failure" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the updated CA CRL to the truststore, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -a -R -f $(PEGASUS_HOME)/testca2.crl >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate, this should fail with a connection failure" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate, this should fail with a connection failure" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Remove the CA CRL certificate" >> $(RESULTFILE)
	@ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TestCA >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	 
	@$(ECHO) "Remove the CA CRL that we just removed, this should get a DNE error" >> $(RESULTFILE)
	@ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TestCA >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)       
	
	@$(ECHO) "Remove the CA certificate from the truststore to clean everything up, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TestCA -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate issued by the CA, this should get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate issued by the CA, this should get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)	
ifndef PEGASUS_PAM_AUTHENTICATION
    ifneq ($(OS), zos)
	cimuser -r -u $(CURRENT_USER)
    endif
endif
	@$(ECHO) ++++ssltrustmgr tests completed
endif

depend:

general:

strip-license:

prepend-license:

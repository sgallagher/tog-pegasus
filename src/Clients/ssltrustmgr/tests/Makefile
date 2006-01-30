#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
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
	@$(RM) $(PEGASUS_HOME)/testexpired1.cert
	@$(RM) $(PEGASUS_HOME)/testexpired1.key
	@$(RM) $(PEGASUS_HOME)/testnotyetvalid1.cert
	@$(RM) $(PEGASUS_HOME)/testnotyetvalid1.key 
	@$(RM) $(PEGASUS_HOME)/testca1.cert
	@$(RM) $(PEGASUS_HOME)/testca1.key
	@$(RM) $(PEGASUS_HOME)/testca1.srl
	@$(RM) $(PEGASUS_HOME)/testca1.crl
	@$(RM) $(PEGASUS_HOME)/testca2.crl
	@$(RM) $(PEGASUS_HOME)/testchca.cert
	@$(RM) $(PEGASUS_HOME)/testchca.key
	@$(RM) $(PEGASUS_HOME)/testchca.srl
	@$(RM) $(PEGASUS_HOME)/testinter.cert
	@$(RM) $(PEGASUS_HOME)/testinter.key
	@$(RM) $(PEGASUS_HOME)/testinter.srl
	@$(RM) $(PEGASUS_HOME)/testleaf.cert
	@$(RM) $(PEGASUS_HOME)/testleaf.key
	@$(RM) index.txt
	@$(RM) index.txt.attr
	@$(RM) index.txt.old
	@$(RM) index.txt.attr.old
	@$(RM) .rnd
	@$(RM) $(PEGASUS_HOME)/ssltest_setup

#
# Create certificates and CRLs required for running the tests
#
test_setup: $(PEGASUS_HOME)/ssltest_setup 

$(PEGASUS_HOME)/ssltest_setup: $(OPENSSL_CNF) testca1.txt testdn1.txt testdn2.txt testdn3.txt testdn4.txt testexpired1.cert testexpired1.key testnotyetvalid1.cert testnotyetvalid1.key testchca.cert testchca.key testinter.cert testinter.key testleaf.cert testleaf.key
	@$(MAKE) -i -s clean
	@$(TOUCH) $(PEGASUS_HOME)/ssltest_setup
ifdef PEGASUS_HAS_SSL
	@#
	@# Copy the expired certificate to the staging area
	@#
	@$(COPY) testexpired1.cert $(PEGASUS_HOME)/testexpired1.cert
	@$(COPY) testexpired1.key $(PEGASUS_HOME)/testexpired1.key
	@#
	@# Create the not yet valid certificate to the staging area
	@#   
	@$(COPY) testnotyetvalid1.cert $(PEGASUS_HOME)/testnotyetvalid1.cert
	@$(COPY) testnotyetvalid1.key $(PEGASUS_HOME)/testnotyetvalid1.key
	@#
	@# Copy the test chain root certificate to the staging area
	@#
	@$(COPY) testchca.cert $(PEGASUS_HOME)/testchca.cert
	@$(COPY) testchca.key $(PEGASUS_HOME)/testchca.key
	@#
	@# Copy the test chain intermediate certificate to the staging area
	@#   
	@$(COPY) testinter.cert $(PEGASUS_HOME)/testinter.cert
	@$(COPY) testinter.key $(PEGASUS_HOME)/testinter.key
	@#
	@# Copy the leaf certificate to the staging area
	@#   
	@$(COPY) testleaf.cert $(PEGASUS_HOME)/testleaf.cert
	@$(COPY) testleaf.key $(PEGASUS_HOME)/testleaf.key
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

SSLCertificateTest1: test_setup
ifdef PEGASUS_HAS_SSL
	@$(RM) $(RESULTFILE)
	@$(MAKE) -i -s SSLCertificateTest1_ignoreerror
	@$(STRIPCRS)
ifdef PEGASUS_OVERRIDE_SSL_CERT_VERIFICATION_RESULT
	@$(DIFF) result.master.override result
else
	$(COMPARERESULTS)
endif
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
	
	@$(ECHO) "Connect with an expired certificate -- should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testexpired1.cert $(PEGASUS_HOME)/testexpired1.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the expired certificate to the truststore -- should get an expiration error" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testexpired1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)	
	
	@$(ECHO) "Connect with a not yet valid certificate -- should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testnotyetvalid1.cert $(PEGASUS_HOME)/testnotyetvalid1.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)  
	
	@$(ECHO) "Add the not yet valid certificate to the truststore -- should get a not yet valid error" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testnotyetvalid1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)  

	@$(ECHO) "Connect with an untrusted certificate -- should fail" >> $(RESULTFILE)
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
	
	@$(ECHO) "Try to connect, this should fail" >> $(RESULTFILE)
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
	
	@$(ECHO) "Try to connect with the 1st certificate, this should fail" >> $(RESULTFILE)
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
	
	@$(ECHO) "Try to connect with the 1st certificate, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate, this should fail" >> $(RESULTFILE)
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
	
	@$(ECHO) "Try to connect with the 1st certificate issued by the CA, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate issued by the CA, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)	

	@$(ECHO) "Add the self-signed root certificate to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testchca.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testchca.cert $(PEGASUS_HOME)/testchca.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with the intermediate certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testinter.cert $(PEGASUS_HOME)/testinter.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with the leaf certificate issued by the CA, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testleaf.cert $(PEGASUS_HOME)/testleaf.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Add the intermediate CA signed certificate to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testinter.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with root CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testchca.cert $(PEGASUS_HOME)/testchca.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with the intermediate certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testinter.cert $(PEGASUS_HOME)/testinter.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with the leaf certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testleaf.cert $(PEGASUS_HOME)/testleaf.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Remove the CA certificate from the truststore, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=California/L=Sacramento/O=OpenGroup/OU=OpenPegasus/CN=TestChainCA -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with the intermediate certificate issued by the CA, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testinter.cert $(PEGASUS_HOME)/testinter.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect with the leaf certificate issued by the CA, this should fail" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testleaf.cert $(PEGASUS_HOME)/testleaf.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Remove the intermediate CA certificate from the truststore, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=California/L=Sacramento/O=OpenGroup/OU=OpenPegasus/CN=TestChainCA -n 2 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Add the root CA certificate to the truststore without an associated username" >> $(RESULTFILE)
	@ssltrustmgr -a -f $(PEGASUS_HOME)/testchca.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Try to connect, this should fail since the root CA does not have an associated user name" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testinter.cert $(PEGASUS_HOME)/testinter.key $(PEGASUS_HOME)/ssl.rnd >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

	@$(ECHO) "Remove the CA certificate from the truststore, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=California/L=Sacramento/O=OpenGroup/OU=OpenPegasus/CN=TestChainCA -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
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

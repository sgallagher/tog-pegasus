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
	
ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
	# This is here because there is a very slight delay between the Windows service starting and the first request being taken
	# This is a separate issue from the tests working
	@CLI gc CIM_ManagedElement >> /dev/null $(REDIRECTERROR)
	@CLI gc CIM_ManagedElement >> /dev/null $(REDIRECTERROR)
	@CLI gc CIM_ManagedElement >> /dev/null $(REDIRECTERROR)
	@CLI gc CIM_ManagedElement >> /dev/null $(REDIRECTERROR)
	@CLI gc CIM_ManagedElement >> /dev/null $(REDIRECTERROR)
endif
	
	@$(ECHO) "Connect with an untrusted certificate -- should get a 401" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn1.cert $(PEGASUS_HOME)/testdn1.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the self-signed certificate to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testdn1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn1.cert $(PEGASUS_HOME)/testdn1.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Attempt to add the same certificate, this should get a duplication error" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testdn1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Delete the self-signed certificate from the truststore" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=JohnDoeSelfSigned/emailAddress=TestCertificate -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect, this should fail and get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn1.cert $(PEGASUS_HOME)/testdn1.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Attempt to delete the certificate we just deleted, this should get a DNE error" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=JohnDoeSelfSigned/emailAddress=TestCertificate -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the CA certificate to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -c $(CURRENT_USER) -f $(PEGASUS_HOME)/testca1.cert >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the CA CRL to the truststore" >> $(RESULTFILE)
	@ssltrustmgr -a -R -f $(PEGASUS_HOME)/testca1.crl >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate, this should fail with a connection failure" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Add the updated CA CRL to the truststore, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -a -R -f $(PEGASUS_HOME)/testca2.crl >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate, this should fail with a connection failure" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate, this should fail with a connection failure" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Remove the CA CRL certificate" >> $(RESULTFILE)
	@ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate, this should succeed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	 
	@$(ECHO) "Remove the CA CRL that we just removed, this should get a DNE error" >> $(RESULTFILE)
	@ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)       
	
	@$(ECHO) "Remove the CA certificate from the truststore to clean everything up, this should succeed" >> $(RESULTFILE)
	@ssltrustmgr -r -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 1st certificate issued by the CA, this should get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn2.cert $(PEGASUS_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 2nd certificate issued by the CA, this should get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn3.cert $(PEGASUS_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	@$(ECHO) "Try to connect with the 3rd certificate issued by the CA, this should get 401'ed" >> $(RESULTFILE)
	@TestCertClient $(PEGASUS_HOME)/testdn4.cert $(PEGASUS_HOME)/testdn4.key >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	
	
	@$(ECHO) ++++ssltrustmgr tests completed
endif

depend:

general:

strip-license:

prepend-license:

ROOT = ../../../..

DIR = Clients/ssltrustmgr/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(TMP_DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master

# Set variables for stopping and starting the cimserver 
# Note that the openssl exe must be in OPENSSL_HOME/bin and on the path for these tests to work, because the createtestcert.sh calls it.

# For testing purposes, the username can be anything, as long as its a valid username
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
CURRENT_USER=Administrator
CIMSERVER_PATH=$(PEGASUS_HOME)/bin/
else    
CURRENT_USER=`whoami`
endif

CIMSERVER_CONFIG_OPTIONS=sslClientVerificationMode=optional sslTrustStoreUserName=$(CURRENT_USER)
TESTCERT_HOME=$(PEGASUS_HOME)/src/Clients/ssltrustmgr/scripts

include $(ROOT)/mak/commands.mak

tests:

install:

clean:
        @$(RM) $(RESULTFILE)

poststarttests:
        @$(RM) $(RESULTFILE)
ifdef PEGASUS_HAS_SSL_NEED_ENABLE
        @$(MAKE) -i -s poststarttests_ignoreerror
        @$(STRIPCRS)
        $(COMPARERESULTS)
        @$(ECHO) +++ Test passed +++
        @$(RM) $(RESULTFILE)
endif

poststarttests_ignoreerror:
        
# enable this once we finish verifying on all OS's
ifdef PEGASUS_HAS_SSL_NEED_ENABLE
        @$(ECHO) ++++ssltrustmgr test
        
        # stop the cimserver and restart with our configuration
        $(TOUCH) $(PEGASUS_HOME)/cimserver_current.conf
        $(COPY) $(PEGASUS_HOME)/cimserver_current.conf $(PEGASUS_HOME)/cimserver_current.conf.bkup
        @$(ECHO) Stopping cimserver
        cd $(PEGASUS_HOME)/bin && $(CIMSERVER_STOP_SERVICE)
        @$(ECHO) Starting cimserver
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
        @$(CIMSERVER_START_SERVICE)
else
        @cd $(PEGASUS_HOME)/bin && $(CIMSERVER_START_SERVICE)
endif
        
        # create the certificates and crls for the tests
        cd ../scripts && bash createtestcert.sh
        @$(ECHO) Past scripts

        @cd $(PEGASUS_HOME)/bin
        
        @$(ECHO) "Connect with an untrusted certificate -- should get a 401" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn1.cert $(TESTCERT_HOME)/testdn1.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Add the self-signed certificate to the truststore" >> $(RESULTFILE)
        @ssltrustmgr -a -c $(CURRENT_USER) -f $(TESTCERT_HOME)/testdn1.cert >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Try to connect, this should be succeed" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn1.cert $(TESTCERT_HOME)/testdn1.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Attempt to add the same certificate, this should get a duplication error" >> $(RESULTFILE)
        @ssltrustmgr -a -c $(CURRENT_USER) -f $(TESTCERT_HOME)/testdn1.cert >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Delete the self-signed certificate from the truststore" >> $(RESULTFILE)
        @ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=JohnDoeSelfSigned/emailAddress=TestCertificate -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Try to connect, this should fail and get 401'ed" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn1.cert $(TESTCERT_HOME)/testdn1.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Attempt to delete the certificate we just deleted, this should get a DNE error" >> $(RESULTFILE)
        @ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=JohnDoeSelfSigned/emailAddress=TestCertificate -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Add the CA certificate to the truststore" >> $(RESULTFILE)
        @ssltrustmgr -a -c $(CURRENT_USER) -f $(TESTCERT_HOME)/testca1.cert >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Try to connect with the 1st certificate issued by the CA, this should succeed" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn2.cert $(TESTCERT_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Try to connect with the 2nd certificate issued by the CA, this should succeed" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn3.cert $(TESTCERT_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Add the CA CRL to the truststore" >> $(RESULTFILE)
        @ssltrustmgr -a -R -f $(TESTCERT_HOME)/testca1.crl >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Attempt to add the same CA CRL again, this should work since you can overwrite CRLs (a warning is sent to the systemlog)" >> $(RESULTFILE)
        @ssltrustmgr -a -R -f $(TESTCERT_HOME)/testca1.crl >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Try to connect with the 1st certificate, this should fail with a connection failure" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn2.cert $(TESTCERT_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
        @TestCertClient $(TESTCERT_HOME)/testdn3.cert $(TESTCERT_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Remove the CA CRL certificate" >> $(RESULTFILE)
        @ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)
        
        # Commenting out until the CRL refresh fully works
        #@$(ECHO) "Try to connect with the 1st certificate, this should succeed" >> $(RESULTFILE)
        #@TestCertClient $(TESTCERT_HOME)/testdn2.cert $(TESTCERT_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
        #@$(ECHO) >> $(RESULTFILE)
        
        #@$(ECHO) "Try to connect with the 2nd certificate, this should succeed" >> $(RESULTFILE)
        #@TestCertClient $(TESTCERT_HOME)/testdn3.cert $(TESTCERT_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
        #@$(ECHO) >> $(RESULTFILE)
 
        @$(ECHO) "Remove the CA CRL that we just removed, this should get a DNE error" >> $(RESULTFILE)
        @ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)       
        
        # Commenting out until the CRL refresh fully works
        #@$(ECHO) "Try to connect with the 1st certificate, this should fail with a 401" >> $(RESULTFILE)
        #@TestCertClient $(TESTCERT_HOME)/testdn2.cert $(TESTCERT_HOME)/testdn2.key >> $(RESULTFILE) $(REDIRECTERROR)
        #@$(ECHO) >> $(RESULTFILE)
        
        #@$(ECHO) "Try to connect with the 2nd certificate, this should fail with a 401" >> $(RESULTFILE)
        #@TestCertClient $(TESTCERT_HOME)/testdn3.cert $(TESTCERT_HOME)/testdn3.key >> $(RESULTFILE) $(REDIRECTERROR)
        #@$(ECHO) >> $(RESULTFILE)
        
        @$(ECHO) "Remove the CA certificate from the truststore to clean everything up, this should succeed" >> $(RESULTFILE)
        @ssltrustmgr -r -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA -n 0 >> $(RESULTFILE) $(REDIRECTERROR)
        @$(ECHO) >> $(RESULTFILE)

        # stop the cimserver and restart using original configuration
        @$(ECHO) Stopping cimserver
        cd $(PEGASUS_HOME)/bin && $(CIMSERVER_STOP_SERVICE)
        $(COPY) $(PEGASUS_HOME)/cimserver_current.conf.bkup $(PEGASUS_HOME)/cimserver_planned.conf
        $(RMDIRHIER) $(PEGASUS_HOME)/truststore
        $(RMDIRHIER) $(PEGASUS_HOME)/clientkeystore
        @$(ECHO) Starting cimserver with previous configuration
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
        @$(CIMSERVER_START_SERVICE)
else
        @cd $(PEGASUS_HOME)/bin && $(CIMSERVER_START_SERVICE)
endif
        @$(ECHO) Running.
        @$(ECHO) ++++ssltrustmgr test completed
endif

depend:

general:

strip-license:

prepend-license:

ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/ssltrustmgr

include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcliutils \
    peggetoopt \
    pegclient \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = ssltrustmgr

SOURCES = \
        SSLTrustMgr.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ws2_32.lib advapi32.lib
endif

# for testing purposes, the username can be anything, as long as its a valid username
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
CURRENT_USER=Administrator
else	
CURRENT_USER=`whoami`
endif

CIMSERVER_CONFIG_OPTIONS=sslClientVerificationMode=optional sslTrustStoreUserName=$(CURRENT_USER)
TESTCERT_HOME=$(PEGASUS_HOME)/src/Clients/ssltrustmgr/scripts

include $(ROOT)/mak/program.mak
include $(ROOT)/mak/install.mak
include $(ROOT)/mak/commands.mak

tests:

poststarttests:

# enable these once we are finish adding the TestClient calls to verify
ifdef PEGASUS_HAS_SSL_NEED_TO_ENABLE
	@$(ECHO) ++++ssltrustmgr test
	$(TOUCH) $(PEGASUS_HOME)/cimserver_current.conf
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf $(PEGASUS_HOME)/cimserver_current.conf.bkup
	@$(ECHO) Stopping cimserver
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_STOP_SERVICE)
	@$(ECHO) Stopped.
	@$(ECHO) Starting cimserver
	@cd $(PEGASUS_HOME)/bin && $(CIMSERVER_START_SERVICE)
	@$(ECHO) Current directory is `pwd`
	
	# create the certificates and crls for the tests
	cd scripts && bash createtestcert.sh
	
	# add the self-signed certificate to the truststore and try to connect
	@cd $(PEGASUS_HOME)/bin && ssltrustmgr -a -c $(CURRENT_USER) -f $(TESTCERT_HOME)/testdn1.cert
	
	# delete the self-signed certificate from the truststore and try to connect
	@cd $(PEGASUS_HOME)/bin && ssltrustmgr -r -i /C=US/ST=VIRGINIA/L=Fairfax/O=OpenGroup/OU=OpenPegasus/CN=JohnDoeSelfSigned/emailAddress=TestCertificate -n 0
	
	# add the CA certificate to the truststore and try to connect using 2 other certificates signed by the CA
	@cd $(PEGASUS_HOME)/bin && ssltrustmgr -a -c $(CURRENT_USER) -f $(TESTCERT_HOME)/testca1.cert
	
	# add the CA CRL certificate to the truststore and try to connect, 1st certificate should succeed, 2nd certificate should fail
	@cd $(PEGASUS_HOME)/bin && ssltrustmgr -a -R -f $(TESTCERT_HOME)/testca1.crl
	
	# remove the CA CRL certificate and try to connect, both attempts should be successful
	@cd $(PEGASUS_HOME)/bin && ssltrustmgr -r -R -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA
	
	# remove the CA certificate from the truststore to clean everything up
	@cd $(PEGASUS_HOME)/bin && ssltrustmgr -r -i /C=US/ST=WASHINGTON/L=Seattle/O=OpenGroup/OU=OpenPegasus/CN=TheMan/emailAddress=TestCA -n 0

	# stop the cimserver and restart using original configuration
	@$(ECHO) Stopping cimserver
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_STOP_SERVICE)
	@$(ECHO) Stopped.
	$(COPY) $(PEGASUS_HOME)/cimserver_current.conf.bkup $(PEGASUS_HOME)/cimserver_planned.conf
	$(RMDIRHIER) $(PEGASUS_HOME)/truststore
	$(RMDIRHIER) $(PEGASUS_HOME)/clientkeystore
	@$(ECHO) Starting cimserver with previous configuration
	cd $(PEGASUS_HOME)/bin && $(CIMSERVER_START_SERVICE)
	@$(ECHO) Running.
	@$(ECHO) ++++ssltrustmgr test completed
endif
	


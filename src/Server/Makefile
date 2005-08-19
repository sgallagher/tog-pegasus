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
ROOT = ../..
DIR = Server
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegclient \
    pegserver \
    peguser \
    pegprm \
    NamespaceProvider \
    pegindicationservice \
    peghandlerservice \
    ConfigSettingProvider \
    DefaultProviderManager \
    ProviderRegistrationProvider \
    pegauthentication \
    UserAuthProvider \
    pegqueryexpression 

ifndef PEGASUS_DISABLE_CQL
    LIBRARIES += \
       pegcql
endif

LIBRARIES += \
    pegquerycommon \
    pegwql

ifndef PEGASUS_DISABLE_CQL
    LIBRARIES += \
       CIMQueryCapabilitiesProvider
endif

LIBRARIES += \
    pegprovidermanager \
    pegprovider \
    pegexportserver \
    pegrepository \
    pegconfig \
    pegcommon \
    pegservice

ifndef PEGASUS_DISABLE_PERFINST
    LIBRARIES += \
       CIMOMStatDataProvider \
       InteropProvider 
else
    ifdef PEGASUS_ENABLE_SLP
        LIBRARIES += \
            InteropProvider 
    endif
endif

ifdef PEGASUS_HAS_SSL
LIBRARIES += \
    CertificateProvider
endif

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifdef PEGASUS_ENABLE_SLP
      LIBRARIES += pegslp
    endif
  endif
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = cimserver

SOURCES = cimserver.cpp

include $(ROOT)/mak/program.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib	
endif

tests:

poststarttests:

include $(ROOT)/mak/commands.mak

PEGASUS_CREATE_SSLCNF_OPTIONS = \
   PEGASUS_SSLCNF_COUNTRY_CODE="UK" \
   PEGASUS_SSLCNF_STATE="Berkshire" \
   PEGASUS_SSLCNF_LOCALITY="Reading" \
   PEGASUS_SSLCNF_ORGANIZATION="The Open Group" \
   PEGASUS_SSLCNF_ORGANIZATION_UNIT="The OpenPegasus Project"

ifdef PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN
PEGASUS_CREATE_SSLCNF_OPTIONS+= \
   PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN=$(PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN)
endif

ifndef PEGASUS_SSLCERT_CNFFILE
   PEGASUS_SSLCERT_CNFFILE=$(PEGASUS_HOME)/ssl.cnf
endif

ifndef PEGASUS_SSLCERT_KEYFILE
   PEGASUS_SSLCERT_KEYFILE=$(PEGASUS_HOME)/file.pem
endif

ifndef PEGASUS_SSLCERT_CERTFILE
   PEGASUS_SSLCERT_CERTFILE=$(PEGASUS_HOME)/cert.pem
endif

PEGASUS_CREATE_SSLCERT_OPTIONS = \
   PEGASUS_SSLCERT_DAYS=3650 \
   PEGASUS_SSLCERT_CNFFILE=$(PEGASUS_SSLCERT_CNFFILE) \
   PEGASUS_SSLCERT_KEYFILE=$(PEGASUS_SSLCERT_KEYFILE)\
   PEGASUS_SSLCERT_CERTFILE=$(PEGASUS_SSLCERT_CERTFILE)

ifdef PEGASUS_SSL_RANDOMFILE
PEGASUS_CREATE_SSL_CERT_OPTIONS += \
   PEGASUS_SSLCERT_RANDOMFILE=$(PEGASUS_HOME)/cimserver.rnd
endif

_createSSLFiles:
	@$(MAKE) -f $(ROOT)/src/Server/Makefile createSSLCnfFile \
            $(PEGASUS_CREATE_SSLCNF_OPTIONS) \
            PEGASUS_SSLCERT_CNFFILE=$(PEGASUS_SSLCERT_CNFFILE)
	@$(MAKE) -f $(ROOT)/src/Server/Makefile createSSLCertificate \
            $(PEGASUS_CREATE_SSLCERT_OPTIONS)

createSSLFilesforCVS-SystemSpecific:
	@$(MAKE) -f $(ROOT)/src/Server/Makefile _createSSLFiles \
            PEGASUS_CREATE_PEGASUS_INTERNAL_CERT=yes \
            PEGASUS_SSLCERT_CNFFILE=$(ROOT)/src/Server/ssl.cnf \
            PEGASUS_SSLCERT_KEYFILE=$(ROOT)/src/Server/file.pem \
            PEGASUS_SSLCERT_CERTFILE=$(ROOT)/src/Server/cert.pem \
            PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN="\"PEGASUS TEST CERTIFICATE-DO NOT USE\""
            
displayCERTFILE:
	@$(ECHO) "*** $(PEGASUS_SSLCERT_CERTFILE) ***"
	@$(MAKE) -f $(ROOT)/src/Server/Makefile displayCertificate \
            PEGASUS_SSLCERT_FILE=$(PEGASUS_SSLCERT_CERTFILE)

cleanupSSLFiles:
	@$(RM) $(PEGASUS_HOME)/cert.pem
	@$(RM) $(PEGASUS_HOME)/file.pem
	@$(RM) $(PEGASUS_HOME)/server.pem
	@$(RM) $(PEGASUS_HOME)/client.pem
	@$(RM) $(PEGASUS_HOME)/cimserver.rnd
	@$(RM) $(PEGASUS_HOME)/ssl.rnd
	@$(RM) $(PEGASUS_HOME)/ssl.cnf
	@$(RM) $(PEGASUS_HOME)/client_cert.pem
	@$(RM) $(PEGASUS_HOME)/client_file.pem
	@$(RMDIRHIER) $(PEGASUS_HOME)/cimserver_trust
	@$(RMDIRHIER) $(PEGASUS_HOME)/indication_trust
	@$(RMDIRHIER) $(PEGASUS_HOME)/crl

createSSLTrustDirectories:
	@$(MKDIRHIER) $(PEGASUS_HOME)/cimserver_trust
	@$(MKDIRHIER) $(PEGASUS_HOME)/indication_trust
	@$(MKDIRHIER) $(PEGASUS_HOME)/crl

stageServerSSLFiles-SystemSpecific:
	@$(MAKE) -f $(ROOT)/src/Server/Makefile cleanupSSLFiles
	@$(COPY) ssl.rnd $(PEGASUS_HOME)
	@$(COPY) cimserver.rnd $(PEGASUS_HOME)
	@$(MAKE) -f $(ROOT)/src/Server/Makefile _createSSLFiles \
            $(PEGASUS_CREATE_SSLCNF_OPTIONS) \
            PEGASUS_SSLCERT_CNFFILE=$(PEGASUS_SSLCERT_CNFFILE)
	@$(COPY) $(PEGASUS_SSLCERT_CERTFILE) $(PEGASUS_HOME)/server.pem
	@$(COPY) $(PEGASUS_SSLCERT_CERTFILE) $(PEGASUS_HOME)/client.pem

stageServerSSLFiles-SystemIndependent:
	@$(MAKE) -f $(ROOT)/src/Server/Makefile cleanupSSLFiles
	@$(COPY) cert.pem $(PEGASUS_HOME)
	@$(COPY) file.pem $(PEGASUS_HOME)
	@$(COPY) cert.pem $(PEGASUS_HOME)/server.pem
	@$(COPY) cert.pem $(PEGASUS_HOME)/client.pem
	@$(COPY) cimserver.rnd $(PEGASUS_HOME)
	@$(COPY) ssl.rnd $(PEGASUS_HOME)
	@$(COPY) ssl.cnf $(PEGASUS_HOME)

stageClientSSLFiles-SystemIndependent:
	@$(COPY) cert.pem $(PEGASUS_HOME)/client_cert.pem
	@$(COPY) file.pem $(PEGASUS_HOME)/client_file.pem
	@$(MAKE) -f $(ROOT)/src/Server/Makefile createSSLTrustDirectories

stageCIMServerConfigurationFiles:
	@$(RM) $(PEGASUS_HOME)/cimserver_planned.conf
	@$(COPY) cimserver_planned.conf $(PEGASUS_HOME)
	
install_run:
	@$(MAKE) -f $(ROOT)/src/Server/Makefile stageServerSSLFiles-SystemIndependent
	@$(MAKE) -f $(ROOT)/src/Server/Makefile stageClientSSLFiles-SystemIndependent
	@$(MAKE) -f $(ROOT)/src/Server/Makefile stageCIMServerConfigurationFiles
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	@$(PEGASUS_HOME)/bin/cimserver -install
endif

uninstall:
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	$(PEGASUS_HOME)/bin/cimserver -remove
endif

run: install_run
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	cimserver -start
else
	cimserver
endif


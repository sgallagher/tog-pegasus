# Copyright (c) 2003 IBM Corporation.
# 
# Author: Subodh Soni (subodh@in.ibm.com)
#
# Makefile for configuring, building and running the Test provider written as
# an enhancement for Pegasus Testing. It is also responsible for creating a new
# Namespace inside $PEGASUS_HOME/repository directory.


ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

SAMPLEPROVIDERNS= root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY= $(REPOSITORY_ROOT)/root\#SampleProvider

LIBRARY = CIMOMSampleProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider
	
SOURCES = \
   CIMOMSampleProvider.cpp \
   CIMOMSampleProviderMain.cpp

include $(PEGASUS_ROOT)/mak/library.mak

client:
	@ echo +++++ Building the Test Client for the Test Providers
	@cd Client
	@$(MAKE) 

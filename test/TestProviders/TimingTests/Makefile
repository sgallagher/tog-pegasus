# Copyright (c) 2003 IBM Corporation.
# 
# Author: Subodh Soni (subodh@in.ibm.com)
# Date: 07-Oct-2003
#
# Makefile for configuring, building and running the Test provider written as
# an enhancement for Pegasus Testing. It is also responsible for creating a new
# Namespace inside $PEGASUS_HOME/repository directory.


ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

SAMPLEPROVIDERNS= root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY= $(REPOSITORY_ROOT)/root\#SampleProvider

LIBRARY = TimingProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider
	
SOURCES = \
   TimingProvider.cpp \
   TimingProviderMain.cpp
LOCAL_DEFINES=-DPEGASUS_DEBUG
#LOCAL_DEFINES=-DNO_OF_INSTANCES=200

include $(PEGASUS_ROOT)/mak/library.mak

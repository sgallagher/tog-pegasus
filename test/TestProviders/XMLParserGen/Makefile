# Copyright (c) 2003 IBM Corporation.
# 
# Author: Subodh Soni (subodh@in.ibm.com)
# Date: 11-Nov-2003
#
# Makefile for configuring, building and running the Test provider written as
# an enhancement for Pegasus Testing. It is also responsible for creating a new
# Namespace inside $PEGASUS_HOME/repository directory.


ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

SAMPLEPROVIDERNS= root/SampleProvider
SAMPLEPROVIDERNSDIRECTORY= $(REPOSITORY_ROOT)/root\#SampleProvider

LIBRARY = ParserTestProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider
	
SOURCES = \
   ParserTestProvider.cpp \
   ParserTestProviderMain.cpp

include $(PEGASUS_ROOT)/mak/library.mak

# Recurse the directories defined in DIRS
# This makefile simply recurses all of the toplevel provider directories

ROOT = ../../..

include $(ROOT)/mak/config.mak

#Directories to recurse
DIRS = \
	PG_TestPropertyTypes \
	DefaultInstanceProvider \
	ClientProvider \
	AlertIndicationProvider \
	ProcessIndicationProvider \
	MCCA_TestAssocProvider \
	InstanceProvider \
	InstanceProvider/testclient \
	UserContextTestProvider \
	UserContextTestProvider/testclient \
	DynamicIndicationProvider

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
ifdef PEGASUS_CMPI_CQL_PROVIDER
DIRS += \
	CMPI/TestIndications \
	CMPI/TestIndications/tests
endif
endif
include $(ROOT)/mak/recurse.mak

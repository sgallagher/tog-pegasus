# Recurse the directories defined in DIRS
# This makefile simply recurses all of the toplevel provider directories

ROOT = ../../..

include $(ROOT)/mak/config.mak

#Directories to recurse
DIRS = \
	AlertIndicationProvider \
	ClientProvider \
	DefaultInstanceProvider \
	DynamicIndicationProvider \
	IndicationStressTestProvider \
	IndicationStressTestProvider/testclient \
	InstanceProvider \
	InstanceProvider/testclient \
	MCCA_TestAssocProvider \
	PG_TestPropertyTypes \
	ProcessIndicationProvider \
	UserContextTestProvider \
	UserContextTestProvider/testclient

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
DIRS += \
	CMPI/TestIndications \
	CMPI/TestIndications/tests
endif
include $(ROOT)/mak/recurse.mak

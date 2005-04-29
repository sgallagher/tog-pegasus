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
	FamilyProvider \
	FamilyProvider/tests \
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
	CMPI/TestIndications/tests \
	CMPI/TestMethod \
	CMPI/TestMethod/tests

ifdef PEGASUS_ENABLE_EXECQUERY
DIRS += \
	CMPI/TestInstance \
	CMPI/TestInstance/tests
endif
endif
include $(ROOT)/mak/recurse.mak

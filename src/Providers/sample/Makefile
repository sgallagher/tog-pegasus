include $(PEGASUS_ROOT)/mak/config.mak

DIRS = \
	InstanceProvider \
	InstanceQueryProvider \
	MethodProvider \
	IndicationProvider \
	SNMPIndicationProvider \
	RT_Indication \
	AssociationProvider \
	AssociationProvider/testclient \
	LocalizedProvider \
	LocalizedProvider/msg

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
DIRS += \
	CMPI/FilesAndDirectories \
	CMPI/FilesAndDirectories/tests
endif

include $(PEGASUS_ROOT)/mak/recurse.mak

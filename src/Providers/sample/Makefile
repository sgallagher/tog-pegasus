include $(PEGASUS_ROOT)/mak/config.mak

DIRS = \
	InstanceProvider \
	MethodProvider \
	IndicationProvider \
	SNMPIndicationProvider \
	ProcessIndicationProvider \
	AlertIndicationProvider \
	RT_Indication \
	FamilyProvider \
	FamilyProvider/tests \
	LocalizedProvider \
	LocalizedProvider/msg
   
ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
ifdef ENABLE_CMPI_PROVIDER_MANAGER
DIRS += \
   CMPI/FilesAndDirectories \
   CMPI/FilesAndDirectories/tests
endif
endif

include $(PEGASUS_ROOT)/mak/recurse.mak


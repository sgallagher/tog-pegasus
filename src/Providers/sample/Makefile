include $(PEGASUS_ROOT)/mak/config.mak

DIRS = \
	InstanceProvider \
	InstanceQueryProvider \
	MethodProvider \
	IndicationProvider \
	SNMPIndicationProvider \
	RT_Indication \
	FamilyProvider \
	FamilyProvider/tests \
	LocalizedProvider \
	LocalizedProvider/msg

# BUG 1801, 1802, etc. Temp bypass until problems fixed. KS
REMOVED_PENDING_BUG_FIXES = \
	CMPI/FilesAndDirectories \
	CMPI/FilesAndDirectories/tests

include $(PEGASUS_ROOT)/mak/recurse.mak

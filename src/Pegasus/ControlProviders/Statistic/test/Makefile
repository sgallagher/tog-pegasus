ROOT=$(PEGASUS_ROOT)
DIR = Pegasus/ControlProviders/Statistic/test
include $(ROOT)/mak/config.mak

LIBRARIES = \
	pegclient \
   	pegcommon

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = testCIMOMStatData
SOURCES = CIMOMStatData.cpp

include $(ROOT)/mak/program.mak

tests:
 
poststarttests:
	$(PROGRAM)

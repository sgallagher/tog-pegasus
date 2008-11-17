ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/EmbeddedInstanceProvider/testClient

include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     peglistener \
     pegclient \
     pegexportserver \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestEmbeddedInstance

SOURCES = EmbeddedInstanceTest.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
ifeq ($(PEGASUS_ENABLE_CQL),true)
	@$(PROGRAM)
endif
	

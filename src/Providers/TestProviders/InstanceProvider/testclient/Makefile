ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/TestProviders/InstanceProvider/testclient

include $(ROOT)/mak/config.mak

#LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegclient \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = TestInstanceProviderClient

SOURCES = TestInstanceProviderClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

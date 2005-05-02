ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/TestProviders/MethodTestProvider/testclient

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegclient \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = TestMethodTestProvider

SOURCES = TestMethodTestProvider.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/TestProviders/UserContextTestProvider/testclient

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon \
    pegclient

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = TestUserContext

SOURCES = TestUserContext.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/tests/OperationResponseHandler

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PPM_INTERNAL -DPEGASUS_INTERNALONLY

PROGRAM = TestOperationResponseHandler

LIBRARIES = \
    pegprovidermanager \
    pegconfig \
    pegprm \
    pegcommon

SOURCES = \
	TestOperationResponseHandler.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:


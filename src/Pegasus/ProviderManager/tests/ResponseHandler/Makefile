include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/Server/tests/ResponseHandler

LIBRARIES = \
	pegcommon \
	pegprovider \
	pegprovidermanager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestResponseHandler

SOURCES = TestResponseHandler.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/Default/tests/ResponseHandler

LIBRARIES = \
    pegprovider \
    pegprovidermanager \
    pegprm \
    pegconfig \
    pegcommon

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestResponseHandler

SOURCES = TestResponseHandler.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:

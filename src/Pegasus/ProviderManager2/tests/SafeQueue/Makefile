include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/tests/SafeQueue

LIBRARIES = \
	pegcommon \
	pegprovidermanager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestSafeQueue

SOURCES = SafeQueue.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

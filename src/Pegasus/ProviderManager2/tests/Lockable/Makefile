include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/tests/Lockable

LIBRARIES = \
	pegcommon \
	pegprovidermanager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestLockable

SOURCES = Lockable.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

ROOT = $(PEGASUS_ROOT)

DIR = Providers/sample/AssociationProvider/testclient

include $(ROOT)/mak/config.mak

PROGRAM = AssociationTestClient

POSTSTARTTESTS =
ifeq ($(OS),HPUX)
  POSTSTARTTESTS = $(PROGRAM)
endif

EXTRA_INCLUDES =

SOURCES = \
	AssociationTestClient.cpp

LIBRARIES = \
	pegprovider \
	pegclient \
	pegcommon

include $(ROOT)/mak/program.mak

general:

tests:

poststarttests:
	$(PROGRAM)

depend:

install:

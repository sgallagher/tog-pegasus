ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/EmbeddedInstanceProvider/testclient

include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     peglistener \
     pegclient \
     pegexportserver \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS),VMS)
 LOCAL_DEFINES = -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)"""
else
 LOCAL_DEFINES = -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"
endif

PROGRAM = TestEmbeddedInstance

SOURCES = EmbeddedInstanceTest.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(PROGRAM)
	

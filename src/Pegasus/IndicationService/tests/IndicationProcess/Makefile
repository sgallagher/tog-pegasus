ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/IndicationProcess
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = IndicationProcess

SOURCES = IndicationProcess.cpp

ifeq ($(OS),VMS)
LOCAL_DEFINES = -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)""" -DPEGASUS_INTERNALONLY
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(PROGRAM)
install:

ROOT = ../../../../../pegasus

DIR = cimservera

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_SECURITY_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon

PRE_DEPEND_INCLUDES = -I./depends

ifeq ($(OS),HPUX)
    EXTRA_LIBRARIES += -lsec 
endif

PROGRAM = cimservera

SOURCES = \
    cimservera.cpp

include $(ROOT)/mak/program.mak

all:
	@$(MAKE)

tests:

poststarttests:


rebuild: clean depend all
	$(MAKE) -s tests

build: depend all
	$(MAKE) -s tests

install:
	$(INSTALL_SBIN_PROGRAM)

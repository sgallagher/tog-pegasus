ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/g11ntest
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
   FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
   SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

include ../libraries.mak

PROGRAM = g11ntest
PROGRAM_NOICU = g11ntest -noicu
SOURCES = g11ntest.cpp

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifdef PEGASUS_ENABLE_SLP
       LIBRARIES += slp
    endif
  endif
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:
ifdef PEGASUS_USE_DEFAULT_MESSAGES
	$(PROGRAM_NOICU)
else
	$(PROGRAM)
endif

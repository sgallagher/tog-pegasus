ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/g11ntest
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
   FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
   SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

include ../libraries.mak
LIBRARIES += peglistener

PROGRAM = g11ntest
PROGRAM_ICU = g11ntest -reqlang en -rsplang en
PROGRAM_NOICU = g11ntest
SOURCES = g11ntest.cpp

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifdef PEGASUS_ENABLE_SLP
       LIBRARIES += pegslp
    endif
  endif
endif

include $(ROOT)/mak/program.mak

install:

tests:

poststarttests:
ifdef PEGASUS_HAS_MESSAGES
	$(PROGRAM_ICU)
else
	$(PROGRAM_NOICU)
endif

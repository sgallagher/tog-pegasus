ROOT = ../../../..

include ../libraries.mak

DIR = Pegasus/Handler/EmailListenerDestination

include $(ROOT)/mak/config.mak

# SSL support
ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LOCAL_DEFINES = -DPEGASUS_HANDLER_INTERNAL -DPEGASUS_INTERNALONLY

SOURCES = EmailListenerDestination.cpp

LIBRARY = EmailListenerDestination
 
include $(ROOT)/mak/library.mak

tests:

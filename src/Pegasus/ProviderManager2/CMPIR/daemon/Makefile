ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/CMPIR/daemon

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) -I .. -I ../native 

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DPEGASUS_DEFAULT_CMPIR -DDEBUG

PROGRAM = CMPIRDaemon

LIBRARIES = CMPIRNative

SOURCES = \
   daemon.c

include $(ROOT)/mak/program.mak


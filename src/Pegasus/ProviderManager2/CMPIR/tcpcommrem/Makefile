ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/CMPIR/tcpcommrem

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) -I .. -I ../tcpcomm -I ../native

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_CMPI_TCPREM_INTERNAL

LIBRARIES = CMPIRNative

LIBRARY = CMPIRTCPCommRemote


SOURCES = \
   tcpcomm_remote.c \
   ../socketcomm.c \
   ../ip.c \
   ../io.c \
   ../serialization.c
   
include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak


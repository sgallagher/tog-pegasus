ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/CMPIR/tcpcomm

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    CXX = cc
    FLAGS += -W"c,SSCOMM"
endif

EXTRA_INCLUDES = $(SYS_INCLUDES) -I ..

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_CMPI_TCP_INTERNAL

LIBRARY = CMPIRTCPComm

LIBRARIES = CMPIRProxyProvider

SOURCES = \
   tcpcomm_proxy.c \
   proxy_attach.c \
   ../serialization.c \
   ../socketcomm.c \
   ../io.c \
   ../ip.c \

include $(ROOT)/mak/library.mak

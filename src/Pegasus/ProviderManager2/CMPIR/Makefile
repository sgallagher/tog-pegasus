ROOT = ../../../..

DIR = Pegasus/ProviderManager2/CMPIR

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) 

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_CMPI_PROXY_INTERNAl 

LIBRARY = CMPIRProxyProvider

LIBRARIES = \
   CMPIProviderManager

SOURCES = \
   proxy_comm.c \
   proxy_provider.c \
   namespace_resolver.c \
   ticket.c \
   tool.c
   
include $(ROOT)/mak/library.mak


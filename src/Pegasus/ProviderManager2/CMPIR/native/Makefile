ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/CMPIR/native

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) -I .. 

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_CMPI_NATIVE_INTERNAL

LIBRARY = CMPIRNative

SOURCES = \
   args.c \
   array.c \
   broker.c \
   broker_os.c \
   context.c \
   datetime.c \
   enumeration.c \
   instance.c \
   objectpath.c \
   property.c \
   result.c \
   selectexp.c \
   string.c \
   value.c \
   remote_broker.c \
   mm.c \
   ../ticket.c \
   ../tool.c \

include $(ROOT)/mak/library.mak


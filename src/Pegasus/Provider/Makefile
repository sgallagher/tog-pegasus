ROOT = ../../..

DIR = Pegasus/Provider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_PROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon \
    pegquerycommon \
    pegclient

LIBRARY = pegprovider

SOURCES = \
    ProviderException.cpp \
    CIMOMHandle.cpp \
    CIMOMHandleRep.cpp \
    CIMOMHandleQueryContext.cpp \
    InternalCIMOMHandleRep.cpp \
    ClientCIMOMHandleRep.cpp \
    CIMProvider.cpp \
    CIMInstanceProvider.cpp \
    CIMMethodProvider.cpp \
    CIMIndicationProvider.cpp \
    CIMIndicationConsumerProvider.cpp \
    CIMQueryProvider.cpp \
    CIMAssociationProvider.cpp \
    CIMNullProvider.cpp \
    CIMInstanceQueryProvider.cpp

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak

ROOT = ../../..

DIR = Pegasus/Provider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_PROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon

LIBRARY = pegprovider

SOURCES = \
    ProviderException.cpp \
    CIMOMHandle.cpp \
    CIMProvider.cpp \
    CIMInstanceProvider.cpp \
    CIMClassProvider.cpp \
    CIMMethodProvider.cpp \
    CIMIndicationProvider.cpp \
    CIMIndicationConsumerProvider.cpp \
    CIMPropertyProvider.cpp \
    CIMQueryProvider.cpp \
    CIMAssociationProvider.cpp \
    CIMNullProvider.cpp

include $(ROOT)/mak/library.mak

ROOT = ../../..

DIR = Pegasus/Provider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_PROVIDER_INTERNAL

LIBRARIES = \
    pegcommon

LIBRARY = pegprovider

SOURCES = \
    CIMOMHandle.cpp \
	OperationFlag.cpp \
    ResponseHandler.cpp \
	SimpleResponseHandler.cpp \
    CIMBaseProvider.cpp \
    CIMInstanceProvider.cpp \
    CIMClassProvider.cpp \
    CIMMethodProvider.cpp \
    CIMIndicationProvider.cpp \
    CIMIndicationConsumer.cpp \
    CIMPropertyProvider.cpp \
    CIMQueryProvider.cpp \
    CIMAssociationProvider.cpp

include $(ROOT)/mak/library.mak

ROOT = ../../..

DIR = Pegasus/Provider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_PROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon

LIBRARY = pegprovider

SOURCES = \
	OperationFlag.cpp \
	ResponseHandler.cpp \
	SimpleResponseHandler.cpp \
    CIMOMHandle.cpp \
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

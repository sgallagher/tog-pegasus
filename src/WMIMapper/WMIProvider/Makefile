ROOT = ../../..

DIR = WMIMapper/WMIProvider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_WMIPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

INCLUDES = -I..

EXTRA_INCLUDES = $(SYS_INCLUDES)

LIBRARY = WMIProvider

LIBRARIES = \
	pegcommon
		
SOURCES = \
   WMIInstanceProvider.cpp \
   WMIClassProvider.cpp \
   WMIAssociatorProvider.cpp \
   WMIQualifierProvider.cpp \
   WMIMethodProvider.cpp \
   WMIReferenceProvider.cpp \
   WMIQueryProvider.cpp \
   WMIBaseProvider.cpp \
   WMICollector.cpp \
   WMIType.cpp \
   WMIFlavor.cpp \
   WMIValue.cpp \
   WMIString.cpp \
   WMIProperty.cpp \
   WMIQualifier.cpp \
   WMIQualifierSet.cpp \
   WMIDateTime.cpp \
   WMIObjectPath.cpp \
   StdAfx.cpp \
   MyString.cpp 

SYS_LIBS = uuid.lib wbemuuid.lib Framedyn.lib ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

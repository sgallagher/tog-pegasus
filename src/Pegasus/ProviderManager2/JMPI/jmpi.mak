ROOT = ../../../..

DIR = Pegasus/ProviderManager2/JMPI

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
   JAVALIBS=$(JAVA_SDK)/jre/lib/i386
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include -I$(JAVA_SDK)/include/linux
   EXTRA_LIBRARIES += -L$(JAVALIBS)/native_threads -L$(JAVALIBS)/client -ljvm -lhpi -lcrypt -lpegclient
endif
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
   JAVALIBS=$(JAVA_SDK)/jre/lib/
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include -I$(JAVA_SDK)/include/win32
   EXTRA_LIBRARIES += $(JAVA_SDK)/lib/jvm.lib
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
   SYS_INCLUDES += -I${JAVA_SDK}/include
   EXTRA_LIBRARIES += ${JAVA_SDK}/bin/classic/libjvm.x
endif

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
 VMS_VECTOR = PegasusCreateProviderManager
endif

LIBRARY = JMPIProviderManager

LIBRARIES = \
    CMPIProviderManager \
    pegprovidermanager \
    pegprovider \
    pegclient \
    pegcommon

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
LIBRARIES += \
	pegclient \
	pegconfig
endif


SOURCES = \
        JMPIProviderManagerMain.cpp \
        JMPIProviderManager.cpp \
        JMPILocalProviderManager.cpp \
        JMPIProviderModule.cpp \
        JMPIProvider.cpp \
        JMPIImpl.cpp

include $(ROOT)/mak/library.mak

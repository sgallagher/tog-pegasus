ROOT = ../../../..

DIR = Pegasus/ProviderManager2/CMPI

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = CMPIProviderManager

LIBRARIES = \
    pegcommon \
	pegprovider \
	pegprovidermanager \
	pegprm

SOURCES = \
	CMPIProviderManagerMain.cpp \
	CMPIProviderManager.cpp \
	CMPILocalProviderManager.cpp \
	CMPIResolverModule.cpp \
	CMPIProviderModule.cpp \
	CMPIProvider.cpp \
	CMPI_Broker.cpp \
	CMPI_ContextArgs.cpp \
	CMPI_Object.cpp \
	CMPI_BrokerEnc.cpp \
	CMPI_Array.cpp \
	CMPI_DateTime.cpp \
	CMPI_Enumeration.cpp \
	CMPI_Instance.cpp \
	CMPI_ObjectPath.cpp \
	CMPI_Result.cpp \
	CMPI_String.cpp \
	CMPI_ThreadContext.cpp \
	CMPI_Value.cpp \
        CMPI_Wql2Dnf.cpp \
        CMPI_SelectExp.cpp \
        CMPI_SelectCond.cpp \
        CMPI_SubCond.cpp \
        CMPI_Predicate.cpp

include $(ROOT)/mak/library.mak

ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

# SSL support
ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_MEMORY_DEBUG

LIBRARY = pegcommon

SOURCES = \
    Array.cpp \
    AsyncOpNode.cpp \
    AuthenticationInfoRep.cpp \
    Base64.cpp \
    CGIQueryString.cpp \
    Char16.cpp \
    CIMClass.cpp \
    CIMClassRep.cpp \
    CIMDateTime.cpp \
    CIMFlavor.cpp \
    CIMIndication.cpp \
    CIMInstance.cpp \
    CIMInstanceRep.cpp \
    CIMMessage.cpp \
    CIMMethod.cpp \
    CIMMethodRep.cpp \
    CIMName.cpp \
    CIMObject.cpp \
    CIMObjectRep.cpp \
    CIMObjectPath.cpp \
    Cimom.cpp \
    CimomMessage.cpp \
    CIMParameter.cpp \
    CIMParameterRep.cpp \
    CIMParamValue.cpp \
    CIMParamValueRep.cpp \
    CIMProperty.cpp \
    CIMPropertyList.cpp \
    CIMPropertyRep.cpp \
    CIMQualifier.cpp \
    CIMQualifierDecl.cpp \
    CIMQualifierDeclRep.cpp \
    CIMQualifierList.cpp \
    CIMQualifierNames.cpp \
    CIMQualifierRep.cpp \
    CIMScope.cpp \
    CIMStatusCode.cpp \
    CIMType.cpp \
    CIMValue.cpp \
    Config.cpp \
    DeclContext.cpp \
    Destroyer.cpp \
    Dir.cpp \
    DQueue.cpp \
    Exception.cpp \
    InternalException.cpp \
    FileSystem.cpp \
    Formatter.cpp \
    HashTable.cpp \
    HTTPAcceptor.cpp \
    HTTPConnection.cpp \
    HTTPConnector.cpp  \
    HTTPMessage.cpp \
    Indentor.cpp \
    IPC.cpp \
    Logger.cpp \
    Memory.cpp \
    Message.cpp \
    MessageQueue.cpp \
    MessageQueueService.cpp \
    ModuleController.cpp \
    Monitor.cpp \
    OperationContext.cpp \
    OperationContextInternal.cpp \
    OptionManager.cpp \
    Pair.cpp \
    Queue.cpp \
    Resolver.cpp \
    ResponseHandler.cpp \
    Service.cpp \
    Sharable.cpp \
    Signal.cpp \
    Socket.cpp \
    Stack.cpp \
    StatisticalData.cpp \
    Stopwatch.cpp \
    String.cpp \
    System.cpp \
    TimeValue.cpp \
    SSLContext.cpp \
    TLS.cpp \
    TraceFileHandler.cpp \
    Tracer.cpp \
    Triad.cpp \
    Thread.cpp \
    Union.cpp \
    XmlParser.cpp \
    XmlReader.cpp \
    XmlWriter.cpp \
    MofWriter.cpp \
    peg_authorization.cpp 

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_PPC_GNU)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_ZSERIES_GNU)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
AIX_LIB_PRIORITY=-20
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl 
  endif
endif

ifeq ($(OS),HPUX)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib
endif

include $(ROOT)/mak/library.mak

# DO NOT DELETE

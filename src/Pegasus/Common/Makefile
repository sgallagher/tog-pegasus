ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

# SSL support
ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegcommon

SOURCES = \
    AnonymousPipe.cpp \
    Array.cpp \
    AsyncOpNode.cpp \
    AuthenticationInfoRep.cpp \
    AutoStreamer.cpp \
    Base64.cpp \
    BinaryStreamer.cpp \
    LanguageParser.cpp \
    LanguageElement.cpp \
    LanguageElementRep.cpp \
    AcceptLanguageElement.cpp \
    ContentLanguageElement.cpp \
    LanguageElementContainer.cpp \
    LanguageElementContainerRep.cpp \
    AcceptLanguages.cpp \
    ContentLanguages.cpp \
    MessageLoader.cpp \
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
    CIMMessageSerializer.cpp \
    CIMMessageDeserializer.cpp \
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
    DynamicLibrary.cpp \
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
    ObjectNormalizer.cpp \
    OperationContext.cpp \
    OperationContextInternal.cpp \
    OptionManager.cpp \
    Pair.cpp \
    QueryExpressionRep.cpp \
    Queue.cpp \
    Resolver.cpp \
    ResponseHandler.cpp \
    ResponseHandlerRep.cpp \
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
    SSLContextManager.cpp \
    TLS.cpp \
    TraceFileHandler.cpp \
    Tracer.cpp \
    Triad.cpp \
    Thread.cpp \
    Union.cpp \
    XmlParser.cpp \
    XmlReader.cpp \
    XmlStreamer.cpp \
    XmlWriter.cpp \
    MofWriter.cpp \
    peg_authorization.cpp \
    CommonUTF.cpp

SOURCES_SLP = Attribute.cpp \
    CIMServerDescription.cpp

ifdef PEGASUS_ENABLE_SLP
  SOURCES += $(SOURCES_SLP)
endif


ifeq ($(OS),linux)
  EXTRA_LIBRARIES += -lcrypt
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
    EXTRA_LINK_ARGUMENTS =  -Xlinker -rpath -Xlinker $(OPENSSL_HOME)/lib
  endif
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif


ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
AIX_LIB_PRIORITY=-20
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(OS),HPUX)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_GNU)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib netapi32.lib
  ifdef PEGASUS_HAS_SSL
    SYS_LIBS += /libpath:$(OPENSSL_HOME)/lib libeay32.lib ssleay32.lib
  endif
endif

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak
# DO NOT DELETE

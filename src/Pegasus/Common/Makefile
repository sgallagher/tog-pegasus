#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
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
    Char16.cpp \
    CIMClass.cpp \
    CIMClassRep.cpp \
    CIMDateTime.cpp \
    CIMFlavor.cpp \
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
    IndicationFormatter.cpp \
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
    Packer.cpp \
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

# Note that PEGASUS_WINDOWS_SDK_HOME only needs to be specified if using VC6.
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib netapi32.lib
  ifdef PEGASUS_HAS_SSL
    SYS_LIBS += /libpath:$(OPENSSL_HOME)/lib libeay32.lib ssleay32.lib
  endif
  ifdef PEGASUS_WINDOWS_SDK_HOME
    FLAGS += -DPEGASUS_WINDOWS_SDK_HOME
    SYS_LIBS += /libpath:$(PEGASUS_WINDOWS_SDK_HOME)/lib secur32.lib
  else
     ifneq ($(CL_MAJOR_VERSION), 12)
    SYS_LIBS += secur32.lib
     endif  
  endif
endif

include $(ROOT)/mak/library.mak
# DO NOT DELETE

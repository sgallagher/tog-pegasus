ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL

LIBRARY = pegcommon

SOURCES = \
    Array.cpp \
    AsyncOpNode.cpp \
    AsyncResponseHandler.cpp \
    AuthenticationInfo.cpp \
    CGIQueryString.cpp \
    Channel.cpp \
    Char16.cpp \
    CIMClass.cpp \
    CIMClassRep.cpp \
    CIMDateTime.cpp \
    CIMFlavor.cpp \
    CIMHandle.cpp \
    CIMIndication.cpp \
    CIMInstance.cpp \
    CIMInstanceRep.cpp \
    CIMMessage.cpp \
    CIMMethod.cpp \
    CIMMethodRep.cpp \
    CIMName.cpp \
    CIMNamedInstance.cpp \
    CIMObject.cpp \
    CIMOMHandle.cpp \
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
    CIMReference.cpp \
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
    Monitor.cpp \
    OperationContext.cpp \
    OptionManager.cpp \
    Pair.cpp \
    Queue.cpp \
    ResponseHandler.cpp \
    Selector.cpp \
    Service.cpp \
    Sharable.cpp \
    Socket.cpp \
    Stack.cpp \
    Stopwatch.cpp \
    String.cpp \
    System.cpp \
    TCPChannel.cpp \
    TimeValue.cpp \
    TraceFileHandler.cpp \
    Tracer.cpp \
    Thread.cpp \
    Union.cpp \
    XmlParser.cpp \
    XmlReader.cpp \
    XmlWriter.cpp
#    AsyncOpNodeLocal.cpp \


SOURCES += TLS.cpp

##
## Took these out again. They broke the NT build.
##


ifneq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SOURCES += MT_MessageQueue.cpp
ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
SOURCES += Signal.cpp
endif
endif

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

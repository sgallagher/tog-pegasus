ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL

LIBRARY = pegcommon

SOURCES = \
    Array.cpp \
    AsyncOpNode.cpp \
    AuthenticationInfo.cpp \
    CGIQueryString.cpp \
    Char16.cpp \
    CIMClass.cpp \
    CIMClassRep.cpp \
    CIMDateTime.cpp \
    CIMFlavor.cpp \
    CIMIndication.cpp \
    CIMInstance.cpp \
    CIMInstanceRep.cpp \
    CIMNamedInstance.cpp \
    CIMMessage.cpp \
    CIMMethod.cpp \
    CIMMethodRep.cpp \
    CIMName.cpp \
    CIMObject.cpp \
    CIMObjectRep.cpp \
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
    MessageQueueService.cpp \
    Monitor.cpp \
    OperationContext.cpp \
    OptionManager.cpp \
    Pair.cpp \
    Queue.cpp \
    Service.cpp \
    Sharable.cpp \
    Socket.cpp \
    Stack.cpp \
    Stopwatch.cpp \
    String.cpp \
    System.cpp \
    TimeValue.cpp \
    TLS.cpp \
    TraceFileHandler.cpp \
    Tracer.cpp \
    Thread.cpp \
    Union.cpp \
    XmlParser.cpp \
    XmlReader.cpp \
    XmlWriter.cpp

##
## Took these out again. They broke the NT build.
##

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
SOURCES += Signal.cpp
endif

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

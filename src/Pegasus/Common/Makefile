ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL

LIBRARY = pegcommon

NEW_SOURCES = \
    Monitor.cpp \
    HTTPAcceptor.cpp \
    HTTPConnection.cpp

SOURCES = \
    $(NEW_SOURCES) \
    OptionManager.cpp \
    Socket.cpp \
    CIMStatusCode.cpp \
    CIMMessage.cpp \
    Message.cpp \
    MessageQueue.cpp \
    CIMObject.cpp \
    Array.cpp \
    Channel.cpp \
    Char16.cpp \
    Config.cpp \
    CIMClass.cpp \
    CIMClassRep.cpp \
    CIMDateTime.cpp \
    CIMFlavor.cpp \
    CIMInstance.cpp \
    CIMInstanceRep.cpp \
    CIMMethod.cpp \
    CIMMethodRep.cpp \
    CIMName.cpp \
    CIMOperations.cpp \
    CIMParameter.cpp \
    CIMParameterRep.cpp \
    CIMProperty.cpp \
    CIMPropertyRep.cpp \
    CIMQualifier.cpp \
    CIMQualifierDecl.cpp \
    CIMQualifierDeclRep.cpp \
    CIMQualifierList.cpp \
    CIMQualifierNames.cpp \
    CIMQualifierRep.cpp \
    CIMReference.cpp \
    CIMScope.cpp \
    CIMType.cpp \
    CIMValue.cpp \
    CIMIndication.cpp \
    CIMOMHandle.cpp \
    DeclContext.cpp \
    Destroyer.cpp \
    Dir.cpp \
    Exception.cpp \
    FileSystem.cpp \
    Formatter.cpp \
    HashTable.cpp \
    Indentor.cpp \
    Logger.cpp \
    Memory.cpp \
    Pair.cpp \
    Queue.cpp \
    TraceFileHandler.cpp \
    Tracer.cpp \
    Selector.cpp \
    Service.cpp \
    Sharable.cpp \
    Stack.cpp \
    Stopwatch.cpp \
    String.cpp \
    System.cpp \
    TCPChannel.cpp \
    TimeValue.cpp \
    Union.cpp \
    XmlParser.cpp \
    XmlReader.cpp \
    XmlWriter.cpp \
    CGIQueryString.cpp

##
## Took these out again. They broke the NT build.
##
ifdef PEGASUS_CONCURRENT
SOURCES +=  IPC.cpp DQueue.cpp Thread.cpp MT_MessageQueue.cpp
endif

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL

LIBRARY = pegcommon

NEW_SOURCES = \
    AsyncOpNode.cpp \
    AsyncOpNodeLocal.cpp \
    AsyncResponseHandler.cpp \
    Monitor.cpp \
    HTTPAcceptor.cpp \
    HTTPConnection.cpp \
    HTTPConnector.cpp  \
    HTTPMessage.cpp \
    HTTPDelegator.cpp \
	 CIMHandle.cpp \
	 OperationContext.cpp \
	 ResponseHandler.cpp

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
    CIMNamedInstance.cpp \
    CIMParameter.cpp \
    CIMParameterRep.cpp \
    CIMParamValue.cpp \
    CIMParamValueRep.cpp \
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
    DQueue.cpp \
    Exception.cpp \
    FileSystem.cpp \
    Formatter.cpp \
    HashTable.cpp \
    Indentor.cpp \
    IPC.cpp \
    Logger.cpp \
    Memory.cpp \
    Pair.cpp \
    Queue.cpp \
    Thread.cpp \
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
    CGIQueryString.cpp \

##
## Took these out again. They broke the NT build.
##

ifneq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SOURCES += MT_MessageQueue.cpp
endif

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

ROOT=../../../..

include $(ROOT)/mak/config.mak

DIRS = \
    Config \
    HashTable \
    Resolve \
    Acceptor \
    Formatter \
    Abstract \
    Array \
    CGIQueryString \
    ClassDecl \
    Compare \
    DateTime \
    Dir \
    FileSystem \
    InstanceDecl \
    Method \
    Parameter \
    Property \
    Qualifier \
    QualifierDecl \
    QualifierList \
    Queue \
    Stack \
    Stopwatch \
    String \
    String \
    StrToInstName \
    TimeValue \
    Tracer \
    ValidateClass \
    Value \
    XmlDump \
    XmlParser \
    XmlPrint \
    Reference \
    DynLib \
    DynLoad \
    Logger \
    WebServer \
    WebClient

ifeq ($(OS),linux)
DIRS += IPC Thread
endif

include $(ROOT)/mak/recurse.mak

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
#    Tracer \

include $(ROOT)/mak/recurse.mak

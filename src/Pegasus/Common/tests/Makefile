ROOT=../../../..

include $(ROOT)/mak/config.mak

DIRS = \
    Config \
    HashTable \
    Resolve \
    Acceptor \
    Connector \
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
    OptionManager

ifeq ($(OS),linux)
SOURCES += IPC
endif

XDIRS = \
    ValidateClass \
    Resolve \

include $(ROOT)/mak/recurse.mak

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
    Logger

ifeq ($(OS),linux)
SOURCES += IPC
endif

XDIRS = \
    ValidateClass \
    Resolve \

include $(ROOT)/mak/recurse.mak

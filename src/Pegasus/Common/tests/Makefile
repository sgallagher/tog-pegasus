ROOT=../../../..

include $(ROOT)/mak/config.mak

DIRS = \
    Config \
    HashTable \
    Resolve \
    Formatter \
    Abstract \
    Array \
    Base64 \
    CGIQueryString \
    ClassDecl \
    Compare \
    DateTime \
    Dir \
    FileSystem \
    InstanceDecl \
    Method \
    Object \
    Parameter \
    Property \
    Qualifier \
    QualifierDecl \
    QualifierList \
    Queue \
    Stack \
    Stopwatch \
    String \
    StrToInstName \
    Thread \
    ThreadPool \
    TimeValue \
    ToMof \
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
    WebClient \
    ParamValue 

#    MessageQueueService \
#    WebServer \
#    async_callback \

ifneq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
DIRS += DQueue
endif

include $(ROOT)/mak/recurse.mak

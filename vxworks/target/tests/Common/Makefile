ROOT = ../../../..
include $(ROOT)/vxworks/config.mak
include $(ROOT)/mak/config.mak

_DIRS = \
    DynamicLibrary \
    Abstract \
    Array \
    AutoPtr \
    AtomicInt \
    AuthenticationInfo \
    Base64 \
    Buffer \
    CIMError\
    Cimom \
    ClassDecl \
    Compare \
    Config \
    DynLib \
    Flavor \
    HashTable \
    HTTPMessage \
    InstanceDecl \
    IndicationFormatter \
    MessageQueue \
    Method \
    ModuleController \
    Mutex \
    Object \
    ObjectNormalizer \
    OperationContext \
    OptionManager \
    ParamValue \
    Parameter \
    Property \
    Qualifier \
    QualifierDecl \
    QualifierList \
    Resolve \
    Scope \
    SpinLock \
    Stack \
    Stopwatch \
    StrToInstName \
    String \
    StringConversion \
    System \
    ThreadPool \
    TimeValue \
    ToMof \
    ValidateClass \
    Value \
    XmlDump \
    XmlParser \
    XmlPrint \
    List \
    AsyncQueue \
    IDFactory \
    SharedPtr \
    HostAddress \
    HostLocator \
    Thread \
    XmlReader \
    Reference \
    Executor \
    DateTime \
    FileSystem \
    Logger \
    Formatter \
    Dir \
    MessageQueueService \
    async_callback \
    MessageSerializer

BASE = $(PEGASUS_ROOT)/src/Pegasus/Common/tests
DIRS = $(addprefix $(BASE)/,$(_DIRS))

include $(PEGASUS_ROOT)/mak/recurse.mak

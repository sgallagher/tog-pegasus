BASE = $(PEGASUS_ROOT)/src/Pegasus/Common/tests
SH = $(PEGASUS_ROOT)/vxworks/vxexec

_DIRS = \
    Abstract \
    AnonymousPipe \
    Array \
    AutoPtr \
    AuditLogger \
    async_callback \
    AtomicInt \
    AuthenticationInfo \
    Base64 \
    Buffer \
    CIMError\
    Cimom \
    ClassDecl \
    Compare \
    Config \
    DateTime \
    Dir \
    DynLib \
    DynamicLibrary \
    Executor \
    FileLock \
    FileSystem \
    Flavor \
    Formatter \
    HashTable \
    HTTPMessage \
    InstanceDecl \
    IndicationFormatter \
    L10N \
    L10N/msg \
    Logger \
    MessageQueue \
    MessageQueueService \
    MessageSerializer \
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
    Reference \
    Resolve \
    Scope \
    SpinLock \
    Stack \
    Stopwatch \
    StrToInstName \
    String \
    StringConversion \
    System \
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
    XmlReader \
    List \
    AsyncQueue \
    IDFactory \
    SharedPtr \
    HostAddress \
    HostLocator

DIRS = $(addprefix $(BASE)/,$(_DIRS))

define NL


endef

all:
	$(foreach i, $(DIRS), $(MAKE) SHELL=$(SH) -SC $(i) tests $(NL) )

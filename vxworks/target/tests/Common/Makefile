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

BROKEN_TESTS = \
    Tracer \
    AuditLogger \
    L10N \
    L10N/msg \
    FileLock \
    AnonymousPipe

BASE = $(PEGASUS_ROOT)/src/Pegasus/Common/tests
DIRS = $(addprefix $(BASE)/,$(_DIRS))

include $(PEGASUS_ROOT)/mak/recurse.mak

define NL


endef

VXEXEC=$(PEGASUS_ROOT)/vxworks/scripts/vxexec

run:
	$(foreach i, $(DIRS), $(MAKE) SHELL=$(VXEXEC) -SC $(i) tests $(NL) )


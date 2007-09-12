BASE = $(PEGASUS_ROOT)/src/Pegasus/Common/tests
SH = $(PEGASUS_ROOT)/vxworks/vxexec

_DIRS1 = \
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
    FileSystem

_DIRS2 = \
    Logger \
    MessageSerializer \
    Tracer \
    Formatter \
    Dir \
    async_callback \
    AuditLogger \
    L10N \
    L10N/msg \
    AnonymousPipe \
    MessageQueueService \

## The following tests are unsupported on VxWorks:
##
##     FileLock - this tests uses popen(), which is unsupported on vxworks.
##

DIRS1 = $(addprefix $(BASE)/,$(_DIRS1))
DIRS2 = $(addprefix $(BASE)/,$(_DIRS2))

define NL


endef

all: dirs1 dirs2

dirs1:
	$(foreach i, $(DIRS1), $(MAKE) SHELL=$(SH) -SC $(i) tests $(NL) )

dirs2:
	$(foreach i, $(DIRS2), $(MAKE) SHELL=$(SH) -SC $(i) tests $(NL) )

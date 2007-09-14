ROOT = ../../../..
include $(ROOT)/vxworks/config.mak
include $(ROOT)/mak/config.mak

_DIRS = \
    Inst \
    Repository \
    Repository2 \
    RepositoryMix \
    NameSpace \
    NameSpaceManager \
    AssocTable \
    EnumerateClassNames \
    InheritanceTree \
    QualifierDeclRep \
    ClassDeclRep \
    SharedNameSpace \
    SharedInheritanceTree \
    CompareRepositories \
    CompareXmlBin \
    CompareXmlCompressed \
    AssocOperations


BASE = $(PEGASUS_ROOT)/src/Pegasus/Repository/tests
DIRS = $(addprefix $(BASE)/,$(_DIRS))

include $(PEGASUS_ROOT)/mak/recurse.mak

define NL


endef

VXEXEC=$(PEGASUS_ROOT)/vxworks/scripts/vxexec

run:
	$(foreach i, $(DIRS), $(MAKE) SHELL=$(VXEXEC) -SC $(i) tests $(NL) )

BASE = $(PEGASUS_ROOT)/src/Pegasus/Common/tests
DIRS = $(BASE)/String $(BASE)/Buffer
SH = $(PEGASUS_ROOT)/vxworks/vxexec

define NL


endef

all:
	$(foreach i, $(DIRS), $(MAKE) SHELL=$(SH) -SC $(i) tests $(NL) )

ifeq ($(PEGASUS_OS_TYPE),VMS)
#
# Make this a program instead of a library.
# VMS can only share executables!
#

PROGRAM = $(addprefix lib,$(LIBRARY))
VMSSHARE =/share/symbol_table=$(OBJ_VMSDIRA)]/sysexe
SHARE_COPY = YES

include $(ROOT)/mak/program.mak

tests: $(ERROR)

#l10n
messages: $(ERROR)

poststarttests: $(ERROR)

else

include $(ROOT)/mak/library.mak

endif


CIMMOFCLI = cimmofl

# The INTEROPNS variable defines the absolute name of the
# Pegasus InterOp Namespace.

INTEROPNS=root/PG_InterOp
INTEROPNSDIRECTORY = $(REPOSITORY_ROOT)/root\#PG_InterOp

# The INTERNALNS variable defines the absolute name of the
# Pegasus INTERNALNS Namespace.

INTERNALNS = root/PG_Internal
INTERNALNSDIRECTORY = $(REPOSITORY_ROOT)/root\#PG_Internal

# The MANAGEDSYSTEMNS variable defines the absolute name of the
# namespace for the managed system running the CIMOM.  The content and
# the name of this namespace is expected to be platform specific.

MANAGEDSYSTEMNS = NOTDEFINED

ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
#   MANAGEDSYSTEMNS = root/HPV2
#   MANAGEDSYSTEMNSDIRECTORY = $(REPOSITORY_ROOT)/root\#HPV2
endif

ifeq ($(MANAGEDSYSTEMNS),NOTDEFINED) 
   MANAGEDSYSTEMNS = root/cimv2
   MANAGEDSYSTEMNSDIRECTORY = $(REPOSITORY_ROOT)/root\#cimv2
endif

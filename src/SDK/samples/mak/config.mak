ECHO = echo

VALID_PLATFORMS = \
    WIN32_IX86_MSVC \
    LINUX_IX86_GNU \
    LINUX_PPC_GNU \
    LINUX_IA64_GNU \
    LINUX_ZSERIES_GNU \
    AIX_RS_IBMCXX \
    HPUX_PARISC_ACC \
    HPUX_IA64_ACC \
    TRU64_ALPHA_DECCXX \
    SOLARIS_SPARC_GNU \
    SOLARIS_SPARC_CC \
    ZOS_ZSERIES_IBM \
    NSK_NONSTOP_NMCPLUS

ifndef PEGASUS_PLATFORM
  ERROR = pegasus_undefined
pegasus_undefined:
	@ $(ECHO) PEGASUS_PLATFORM environment variable undefined. Please set to\
	one of the following: $(VALID_PLATFORMS)
	@ exit 1
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_PPC_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_ZSERIES_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

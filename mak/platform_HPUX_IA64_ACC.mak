include $(ROOT)/mak/config-unix.mak
include $(ROOT)/mak/platform_HPUX_ACC.mak

ARCHITECTURE = IA64

ifeq ($(HPUX_IA64_NATIVE_COMPILER), yes)
  SYS_INCLUDES += 
else
  SYS_INCLUDES += -I$(ROOT)/src/stdcxx/stream
endif

DEFINES += -DPEGASUS_ARCHITECTURE_IA64
ifeq ($(HPUX_IA64_NATIVE_COMPILER), yes)
  DEFINES += -DHPUX_IA64_NATIVE_COMPILER
endif

FLAGS += +Z +DD64 -AA -mt 

ifdef PEGASUS_PAM_AUTHENTICATION
  SYS_LIBS += -L$(PAMLIB_HOME) -lpam
endif

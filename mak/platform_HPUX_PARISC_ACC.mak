include $(ROOT)/mak/config-unix.mak
include $(ROOT)/mak/platform_HPUX_ACC.mak

ARCHITECTURE = PARISC

SYS_INCLUDES += -I$(ROOT)/src/stdcxx/stream

FLAGS += +Z +DAportable -mt -D_PSTAT64

ifdef PEGASUS_PAM_AUTHENTICATION
   SYS_LIBS += -lpam
endif

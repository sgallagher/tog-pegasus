ifeq ($(PLATFORM_VERSION_SUPPORTED), yes)
  ifdef PLATFORM_COMPONENT_NAME
     DEFINES += -DPLATFORM_COMPONENT_NAME=\"$(PLATFORM_COMPONENT_NAME)\"
  else
     DEFINES += -DPLATFORM_COMPONENT_NAME=\"$(PROGRAM)\"
  endif
endif

include $(ROOT)/mak/common.mak

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/program-windows.mak
endif
ifeq ($(OS_TYPE),unix)
include $(ROOT)/mak/program-unix.mak
endif
ifeq ($(OS_TYPE),nsk)
include $(ROOT)/mak/program-nsk.mak
endif

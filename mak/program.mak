ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/program-windows.mak
endif
ifeq ($(OS_TYPE),unix)
include $(ROOT)/mak/program-unix.mak
endif
ifeq ($(OS_TYPE),nsk)
include $(ROOT)/mak/program-nsk.mak
endif
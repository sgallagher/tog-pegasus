ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/objects-windows.mak
endif
ifeq ($(OS_TYPE),unix)
include $(ROOT)/mak/objects-unix.mak
endif
ifeq ($(OS_TYPE),nsk)
include $(ROOT)/mak/objects-nsk.mak
endif

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/objects-windows.mak
else
include $(ROOT)/mak/objects-unix.mak
endif

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/depend-windows.mak
else
include $(ROOT)/mak/depend-unix.mak
endif

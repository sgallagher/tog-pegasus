ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = WMIMapper/WMIOP

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS_TYPE),windows)
EXTRA_INCLUDES += -I./nt_includes
endif

LIBRARIES = \
    pegwmiclient \
    pegcommon

PROGRAM = wmiop

SOURCES = WMIop.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

ROOT = ../../../../../..
DIR = Pegasus/Common/tests/Service/BeeperService
include $(ROOT)/mak/config.mak
LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

DYNAMIC_LIBRARIES = -lpegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = TestBeeperService

SOURCES = BeeperService.cpp

include $(ROOT)/mak/program.mak

SERVICE_PATH = e:/tog/pegasus_home/bin/BeeperService.exe

tests:

install:
	- InstallService $(SERVICE_PATH) beeper "Beeper Service"

remove:
	- RemoveService beeper

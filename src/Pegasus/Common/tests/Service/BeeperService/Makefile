ROOT = ../../../../../..
DIR = Pegasus/Common/tests/Service/BeeperService
include $(ROOT)/mak/config.mak
LIBRARIES = pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestBeeperService

SOURCES = BeeperService.cpp

include $(ROOT)/mak/program.mak

SERVICE_PATH = e:/tog/pegasus_home/bin/BeeperService.exe

tests:

install:
	- InstallService $(SERVICE_PATH) beeper "Beeper Service"

remove:
	- RemoveService beeper

ROOT=../../../..

include $(ROOT)/mak/config.mak

DIRS = \
	ConfigFileHandler \
	ConfigManager

ifeq ($(OS),linux)
SOURCES += IPC
endif

include $(ROOT)/mak/recurse.mak

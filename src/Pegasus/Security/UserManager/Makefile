ROOT = ../../../..

DIR = Pegasus/Security/UserManager

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_USERMANAGER_INTERNAL

LIBRARY = peguser

include ./libraries.mak

SOURCES = \
        UserExceptions.cpp \
	PasswordFile.cpp \
	UserFileHandler.cpp \
	AuthorizationHandler.cpp \
	UserManager.cpp 

include $(ROOT)/mak/library.mak

include $(ROOT)/mak/config-nsk.mak   

LIB_SUFFIX = .a

OS = nsk   

ARCHITECTURE = NonStop

COMPILER = nmcplus

SYS_INCLUDES = 

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =                         

ifdef PEGASUS_DEBUG
FLAGS = -g -Winspect -Wversion2 -Werrors=10
else
FLAGS = -Wversion2 -Werrors=10
endif

GUARDIAN_FLAGS = -Wextensions

SYS_LIBS = /usr/lib/crtlmain.o -obey /usr/lib/libc.obey -l zrwslsrl -l zcplsrl -l zstfnsrl -l ztlhsrl -verbose      

CXX = c89 

LXX = nld

SH = sh

YACC = yacc 

COPY = cp

MOVE = mv

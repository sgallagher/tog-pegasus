ROOT = ../../..

DIR = Pegasus/Compiler

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)

ifeq ($(OS_TYPE),windows)
EXTRA_INCLUDES += -I./nt_includes
endif

LOCAL_CLEAN_TARGET = local_clean_target

LOCAL_DEFINES = -DPEGASUS_COMPILER_INTERNAL

LIBRARY = pegcompiler

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX)
#    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
#    $(ACE_LIB)

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = cimmof_lex.cpp cimmofParser.cpp \
	cimmofRepository.cpp cimmof_tab.cpp parser.cpp \
	qualifierList.cpp valueFactory.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak

cimmof_tab.cpp cimmof_tab.h: cimmof.y
	$(YACC) -vd -ocimmoftemp -p cimmof_ cimmof.y
	$(COPY) cimmoftemp cimmof_tab.cpp
	$(COPY) cimmoftemp.h cimmof_tab.h
	$(RM) cimmoftemp cimmoftemp.h

cimmof_lex.cpp: cimmof.l cimmof_tab.h
	$(LEX) -ocimmof_lex.cpp -Pcimmof_ cimmof.l

local_clean_target:
	$(RM) cimmof_lex.cpp cimmof_tab.cpp cimmof_tab.h

#
#  NOTE:  The grammar and parser are no longer built in this makefile,
#  and in fact flex and bison are assumed to already have been run 
#  and their output committed to CVS.  Run 
#      make -f Makefile.grammmar
#  to generate cimmof_tab.cpp, cimmof_tab.h, and cimmof_lex.cpp, 
#  and remember to  commit them afterwards.
#

ROOT = ../../..

DIR = Pegasus/Compiler

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS_TYPE),windows)
EXTRA_INCLUDES += -I./nt_includes
endif

LOCAL_CLEAN_TARGET = local_clean_target

LOCAL_DEFINES = -DPEGASUS_COMPILER_INTERNAL

LIBRARY = pegcompiler

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX)

DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegrepository

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = cimmof_lex_wrapper.cpp cimmofParser.cpp cimmofMessages.cpp \
	cimmofRepository.cpp cimmof_tab_wrapper.cpp parser.cpp \
	qualifierList.cpp valueFactory.cpp objname.cpp compilerDeclContext.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak


$(OBJ_DIR)/cimmof_tab_wrapper$(OBJ) : cimmof_tab.cpp

$(OBJ_DIR)/cimmof_lex_wrapper$(OBJ) : cimmof_lex.cpp

local_clean_target:
	$(RM) cimmoftemp.out cimmoftemp.output

################################################################################
##
################################################################################

INCLUDES = "/Pegasus_Blddsk","$(ROOT)/src","$(ROOT)/src/standardincludes/cincludes$(EXTRA_VMSINCLUDES)

DEFINES +=,__USE_STD_IOSTREAM,PEGASUS_USE_RELEASE_DIRS,PEGASUS_USE_RELEASE_CONFIG_OPTIONS

$(OBJ_DIR)/%.obj: %.c $(ERROR)
	$(CC)/object=$(OBJ_VMSDIRA)] $(CCFLAGS)/DEFINE=(__NEW_STARLET) $*.c$(VMS_CLIB)
	@ $(TOUCH) $(OBJ_VMSDIRA)]$*.obj
	@ $(ECHO)

$(OBJ_DIR)/%.obj: %.cpp $(ERROR)
	$(CXX)/object=$(OBJ_VMSDIRA)] $(CFLAGS)/define=(VMS$(TMP_FLAGS)$(TMP_DEFINES)$(TMP_LDEFINES))/include=($(INCLUDES)$(TMP_SINCLUDES)","$(OPENSSL_VMSHOME)") $*.cpp$(VMS_LIB)
ifndef EXE_OUTPUT
	library/replace $(FULL_VMSLIB) $(OBJ_VMSDIRA)]$*.obj
endif
	@ $(TOUCH) $(OBJ_VMSDIRA)]$*.obj
	@ $(ECHO)

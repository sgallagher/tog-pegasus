################################################################################
##
## An ugly trick is used here to overcome a bug in g++ 2.9.5. G++ failes to
## cleanup object files that are placed in directories other than the
## current one (using the -o option). To overcome this bug, we use -o.tmp.o 
## and then move the file to the object directory.
##
################################################################################

ifeq ($(OS),HPUX)
  _NO_TMP_O = yes
endif

ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
  _NO_TMP_O = yes
endif

_TMP_O = $(PEGASUS_PLATFORM).o

ifeq ($(_NO_TMP_O), yes)
$(OBJ_DIR)/%.o: %.cpp $(ERROR)
	$(CXX) -c -o $@ $(FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	@ $(TOUCH) $@
	@ $(ECHO)
else
$(OBJ_DIR)/%.o: %.cpp $(ERROR)
	$(CXX) -c -o $(_TMP_O) $(FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	@ $(COPY) $(_TMP_O) $@
	@ $(RM) $(_TMP_O)
	@ $(TOUCH) $@
	@ $(ECHO)
endif

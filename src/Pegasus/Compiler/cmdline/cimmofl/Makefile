ROOT = ../../../../..
DIR = Pegasus/Compiler/cmdline/cimmofl
include $(ROOT)/mak/config.mak

PROGRAM = cimmofl
CIMMOF = $(BIN_DIR)/cimmof$(EXE)

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)


$(FULL_PROGRAM): $(CIMMOF)
	$(COPY) $(CIMMOF) $(FULL_PROGRAM)

clean:
	$(RM) $(FULL_PROGRAM)

tests:

poststarttests: 

depend:

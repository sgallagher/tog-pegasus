PROJECT = slp_srvreg
LFLAGS += -o$@
YFLAGS += -o$@

SOURCES = slp_srvreg.c 

OBJS = slp_srvreg$(OBJ) 

LIBS = $(SRC_CLIENT)/libslp_client.la

all: $(PROJECT)$(EXE)
.PHONY: all

install: $(INSTALL) install.txt
.PHONY: install

clean: $(CLEAN) clean.txt
.PHONY: clean

$(PROJECT)$(EXE): slp_srvreg$(OBJ)
	$(LIBTOOL) $(CC) $(LDFLAGS) $(EXE_OUT) $@ $(OBJS) $(LIBS)

slp_srvreg$(OBJ) : slp_srvreg.c 
	$(LIBTOOL) $(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $< $(OBJ_OUT) $@

install.txt:  $(PROJECT)$(EXE)
	$(IGNORE)$(LIBTOOL) $(CP) $(CP_FLAGS) slp_srvreg /usr/local/bin

remove.txt:
	$(IGnore)$(LIBTOOL) $(RM) $(RM_FLAGS) /usr/local/bin/$(SUB_PROJECT)$(EXE)

clean.txt :
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) $(SUB_PROJECT)$(EXE)
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *$(OBJ)










PROJECT = slp_query
LFLAGS += -o$@
YFLAGS += -o$@

SOURCES = slp_query.c 

OBJS = slp_query$(OBJ) 

LIBS = $(SRC_CLIENT)/libslp_client.la

all: $(PROJECT)$(EXE)
.PHONY: all

install: $(INSTALL) install.txt
.PHONY: install

remove: $(REMOVE) remove.txt
.PHONY: remove

clean: $(CLEAN) clean.txt
.PHONY: clean


$(PROJECT)$(EXE): slp_query$(OBJ)
	$(LIBTOOL) $(CC) $(LDFLAGS) $(EXE_OUT) $@ $(OBJS) $(LIBS)

slp_query$(OBJ) : slp_query.c 
	$(LIBTOOL) $(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $< $(OBJ_OUT) $@

clean.txt :
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) $(SUB_PROJECT)$(EXE)
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.o
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.lo

install.txt: $(PROJECT)$(EXE)
	$(IGNORE)$(LIBTOOL) $(CP) $(CP_FLAGS) slp_query /usr/local/bin

remove.txt:
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) /usr/local/bin/slp_query








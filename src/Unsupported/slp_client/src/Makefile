COMPONENTS = all
CLIENT = client
CLEAN = clean
INSTALL = install 
REMOVE = remove

DIRS = 	$(SRC_DIR)/kernel \
	$(SRC_DIR)/cmd-utils

all: $(COMPONENTS)
.PHONY: all 

client: $(CLIENT)
.PHONY: client

clean: $(CLEAN)
.PHONY: clean

install: $(INSTALL)
.PHONY: install

REMOVE: $(REMOVE)
.PHONY: remove


$(COMPONENTS) $(INSTALL) $(REMOVE):
	$(foreach dir, $(DIRS), cd $(dir) && $(MAKE) $@;)
$(CLEAN):
	$(foreach dir, $(DIRS), cd $(dir) && $(MAKE) -k $@;)
$(CLIENT):
	cd $(SRC_DIR)/cmd-utils && $(MAKE)  

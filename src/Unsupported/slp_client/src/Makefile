COMPONENTS = all
KERNEL = kernel 
CLIENT = client
CLEAN = clean
INSTALL = install 
REMOVE = remove

DIRS = 	$(SRC_DIR)/kernel \
	$(SRC_DIR)/cmd-utils

all: $(COMPONENTS)
.PHONY: all 

kernel: $(KERNEL)
.PHONY: kernel

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
$(KERNEL):
	cd $(SRC_DIR)/kernel && $(MAKE)

$(CLIENT):
	cd $(SRC_DIR)/cmd-utils && $(MAKE)  

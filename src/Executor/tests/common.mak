ifeq ($(findstring _GNU, $(PEGASUS_PLATFORM)), _GNU)
    FLAGS += -pedantic -ansi -Werror -Wunused -Wno-unused-function
    CXX=gcc
endif

ROOT=../..

include $(ROOT)/mak/config.mak

DIRS = \
    getoopt \
    Compiler \
    Compiler/cmdline

include $(ROOT)/mak/recurse.mak

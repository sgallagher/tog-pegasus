# Recurse the directories defined in DIRS
# This makefile simply recurses all of the toplevel provider directories

ROOT = ../..

include $(ROOT)/mak/config.mak

#Directories to recurse
DIRS = \
    generic \
    ManagedSystem \
	sample

# ATTN: Comment this out: it was breaking the build:
# DIRS = generic windows unix

include $(ROOT)/mak/recurse.mak

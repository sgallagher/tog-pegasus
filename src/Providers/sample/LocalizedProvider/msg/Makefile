# This is an example of the 'standard' Makefile to compile ICU resource
# bundles.

ROOT = ../../../../..

DIR = Providers/sample/LocalizedProvider/msg

include $(ROOT)/mak/config.mak

# ICU package name for genrb
MSG_PACKAGE = localizedProvider

# Destination directory for the compiled bundles
# Note: MSG_ROOT is set to PEGASUS_HOME/msg by default.
MSG_DIR = $(MSG_ROOT)/provider/localizedProvider

# The list of resource bundles to compile.
# NOTE - your .txt files must be named with the package name before the '_'
MSG_SOURCES = \
    localizedProvider_en.txt \
    localizedProvider_fr.txt \
    localizedProvider_de.txt\
    localizedProvider_es.txt 

# Keep the following in the order listed, and at the end of the file

include $(ROOT)/mak/message.mak

messages: $(MSG_BUNDLES)

# DO NOT DELETE

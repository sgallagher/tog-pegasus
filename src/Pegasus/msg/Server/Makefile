ROOT = ../../../..

DIR = Pegasus/msg/Server

include $(ROOT)/mak/config.mak

# ICU package name
MSG_PACKAGE = pegasusServer 

# Destination directory for the compiled bundles
MSG_DIR = $(MSG_ROOT)/pegasus

# NOTE - your .txt files must be named with the package name before the '_'
MSG_SOURCES = \
    pegasusServer_root.txt \
    pegasusServer_en.txt 

# Keep the following in the order listed

include $(ROOT)/mak/message.mak

messages: $(MSG_BUNDLES)

# DO NOT DELETE

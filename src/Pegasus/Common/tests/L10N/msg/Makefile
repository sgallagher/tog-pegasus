ROOT = ../../../../../..

DIR = Pegasus/Common/tests/L10N/msg

include $(ROOT)/mak/config.mak

# ICU package name
MSG_PACKAGE = pegasusTest


# Destination directory for the compiled bundles
MSG_DIR = $(MSG_ROOT)/test

# NOTE - your .txt files must be named with the package name before the '_'
MSG_SOURCES = \
    pegasusTest_en.txt \
    pegasusTest_en_us.txt \
    pegasusTest_fr.txt \
    pegasusTest_fu_nk.txt \
    pegasusTest_st_at_ic.txt

# Keep the following in the order listed

include $(ROOT)/mak/message.mak

messages: $(MSG_BUNDLES)

# DO NOT DELETE

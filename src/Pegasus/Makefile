ROOT=../..

include $(ROOT)/mak/config.mak

TEST_DIRS = \

# This breaks the build since it has no depend rule: Config/install \
# Also maybe we don't want to install during build.

DIRS = \
    Common \
    Config \
    Repository \
    Protocol \
    getoopt \
    Compiler \
    Compiler/load \
    Compiler/cmdline \
    Client \
    ExportClient \
    Provider \
    Provider2 \
    Handler/CIMxmlIndicationHandler \
    Handler/sendmailIndicationHandler \
    Server \
    Common/tests \
    Config/tests \
    Repository/tests \
    Server/tests \
    Client/tests \
    Compiler/tests

include $(ROOT)/mak/recurse.mak

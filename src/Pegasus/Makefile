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
    ExportServer \
    Server \
    Handler/CIMxmlIndicationHandler \
    Handler/sendmailIndicationHandler \
    Handler/snmpIndicationHandler \
    Common/tests \
    Config/tests \
    Repository/tests \
    Server/tests \
    ExportServer/tests \
    Client/tests \
    Compiler/tests 

include $(ROOT)/mak/recurse.mak

ROOT = ../../../..

include $(ROOT)/mak/config.mak


#
# The Inst test was removed because a fix that enabled it to link
# on Windows and Solaris was removed per instructions from the
# Steering Committee.
#

#
# The LoadRepository test is purposely not included in the list below
# because all of the necessary files were never checked in.  If and
# when someone checks in the rest of the files the test should be
# enabled.
#

DIRS = \
    Repository \
    Repository2 \
    RepositoryMix \
    NameSpace \
    NameSpaceManager \
    AssocTable \
    EnumerateClassNames \
    InheritanceTree \
    QualifierDeclRep \
    ClassDeclRep \
    SharedNameSpace \
    SharedInheritanceTree \
    CompareRepositories \
    CompareXmlBin \
    LoadClass \
    CompareXmlCompressed

include ../../../../mak/recurse.mak

include $(PEGASUS_ROOT)/mak/config.mak

DIRS = \
    lib \
    Config \
    LocalAuth \
    Macro \
    Match \
    Messages \
    PasswordFile \
    Path \
    Process \
    Random \
    Socket \
    Strlcat \
    Strlcpy \
    User

ifdef PEGASUS_PAM_AUTHENTICATION
    DIRS += PAMAuth
endif

include $(PEGASUS_ROOT)/mak/recurse.mak

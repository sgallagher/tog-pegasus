ifndef ROOT
    ROOT =  $(subst \,/,$(PEGASUS_ROOT))
endif

ifndef OS
   include $(ROOT)/mak/config.mak
endif

ifeq ($(OS),win32)
  STRIPCRS = stripcrs $(RESULTFILE) $(MASTERRESULTFILE)
  DIFF = mu compare
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver -start $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -stop
  SLEEP =
  REMOVE_PEGASUS_DIRECTORY = mu rmdirhier pegasus
  MUEXE = mu.exe
  MKDIRHIER = $(MUEXE) mkdirhier
  RMDIRHIER = $(MUEXE) rmdirhier
endif

ifeq ($(OS),HPUX)
  STRIPCRS =
  DIFF = diff
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  RMDIRHIER = rm -rf
endif

ifeq ($(OS),solaris)
  STRIPCRS =
  DIFF = diff
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = /usr/bin/ps -ef | /usr/bin/grep cimserver | /usr/bin/grep -v grep | /usr/bin/awk '{print "kill -9 "$$2 |"/usr/bin/ksh"}'
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = $(MUEXE) mkdirhier
  RMDIRHIER = $(MUEXE) rmdirhier
endif

ifeq ($(OS),linux)
  STRIPCRS =
  DIFF = diff
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
endif

ifeq ($(OS),zos)
  STRIPCRS =
  DIFF =
  REDIRECTERROR =
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
endif

ifeq ($(OS),VMS)
  STRIPCRS =
  DIFF = 
  REDIRECTERROR =
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = wait
  REMOVE_PEGASUS_DIRECTORY = mu rmdirhier pegasus
  MUEXE = mu
  MKDIRHIER = $(MUEXE) mkdirhier
  RMDIRHIER = $(MUEXE) rmdirhier
endif

ifeq ($(OS),aix)
  STRIPCRS =
  DIFF = 
  REDIRECTERROR =
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
endif

ifndef TMP_DIR
 ifdef PEGASUS_TMP
   TMP_DIR = $(subst \,/,$(PEGASUS_TMP))
 else
   TMP_DIR = .
 endif
endif

FORCE: 

cimstop: FORCE
	$(CIMSERVER_STOP_SERVICE)

cimstop_IgnoreError: FORCE
	@make -f $(ROOT)/mak/commands.mak -i cimstop

cimstart: FORCE
	$(CIMSERVER_START_SERVICE)

sleep: FORCE
	$(SLEEP) $(TIME)

mkdirhier: FORCE
	$(MKDIRHIER) $(DIRNAME)

mkdirhier_IgnoreError: FORCE
	@make -f $(ROOT)/mak/commands.mak -i mkdirhier

rmdirhier: FORCE
	$(RMDIRHIER) $(DIRNAME)

rmdirhier_IgnoreError: FORCE
	@make -f $(ROOT)/mak/commands.mak -i rmdirhier

testCommands: FORCE
	$(MAKE) -f $(ROOT)/mak/commands.mak sleep TIME=10
	$(MAKE) -f $(ROOT)/mak/commands.mak rmdirhier_IgnoreError DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak mkdirhier DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak mkdirhier_IgnoreError DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak rmdirhier DIRNAME=$(TMP_DIR)/PegasusTestDirA
	$(MAKE) -f $(ROOT)/mak/commands.mak rmdirhier_IgnoreError DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop_IgnoreError
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstart
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop_IgnoreError
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstart CIMSERVER_CONFIG_OPTIONS="traceLevel=1 traceComponents=XmlIO"
	cimconfig -g traceLevel -c
	cimconfig -g traceComponents -c
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop

ROOT = ../../../../..
DIR = Pegasus/Common/tests/FileSystem
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestFileSystem
SOURCES = FileSystem.cpp

include $(ROOT)/mak/program.mak

LOCAL_FILES_TO_CLEAN = \
	$(TMP_DIR)/TestFile.txt \
	$(TMP_DIR)/TestFile1.txt \
	$(TMP_DIR)/file1.txt

LOCAL_DIRS_TO_CLEAN = \
	$(TMP_DIR)/TestDirectory2

tests:
	$(PROGRAM)
	$(RM) $(LOCAL_FILES_TO_CLEAN)
	$(RMDIRHIER) $(LOCAL_DIRS_TO_CLEAN)

poststarttests:

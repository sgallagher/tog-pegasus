# Test ExecQuery

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
SHH = @
SAMPLEPROVIDERNS=root/SampleProvider

depend:

tests:

messages:

poststarttests:

	@$(ECHO) +++++ Test ExecQuery on sample \"Family\" classes

	$(SHH)CLI xq "select * from TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 14
	$(SHH)CLI xq "select Name from TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 14
	$(SHH)CLI xq "select * from TST_Person where Name = @Mike@ or Name = @Father@" -n "$(SAMPLEPROVIDERNS)" -s -count 2
	$(SHH)CLI xq "select * from TST_Person where Name = @Saara@ and extraProperty = @defaultvalue@" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI xq "select junk from TST_Person" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	$(SHH)CLI xq "select * from TST_PersonDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	$(SHH)CLI xq "select * from TST_PersonDynamicSubClass" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI xq "select instanceCounter from TST_Person where secondProperty = @five@" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI xq "select * from TST_Person where instanceCounter = 6" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI xq "select * from TST_Person where name = @Mike@" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI xq "select * from TST_Person where name = @Father@" -n "$(SAMPLEPROVIDERNS)" -s -count 1
	$(SHH)CLI xq "select * from TST_Person where instanceCounter = 20" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	$(SHH)CLI xq "select * from TST_PersonS" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI xq "select * from TST_PersonS where counter = 2" -n "$(SAMPLEPROVIDERNS)" -s -count 1

	$(SHH)CLI xq "select * from TST_Lineage" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI xq "select parent from TST_Lineage" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI xq "select child from TST_Lineage" -n "$(SAMPLEPROVIDERNS)" -s -count 4
	$(SHH)CLI xq "select * from TST_LabeledLineage where Name = @Michael@" -n "$(SAMPLEPROVIDERNS)" -s -count 0
	$(SHH)CLI xq "select * from TST_LabeledLineageDynamic" -n "$(SAMPLEPROVIDERNS)" -s -count 6
	@$(ECHO) +++++ ExecQuery Tests passed


clean:

install:
# END_OF_FILE

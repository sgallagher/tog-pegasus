ROOT = ../../../../../..

DIR = Pegasus/Query/QueryExpression/tests/Queries


include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
    DIFF = mu compare
else
    DIFF = diff
endif

QueryExpression.query.res: QueryExpression.query
	@ testQueryExpression QueryExpression.query -className QExpr_TestPropertyTypes > $(TMP_DIR)/QueryExpression.query.res | cd .
	@ $(DIFF) QueryExpression.query.resgood $(TMP_DIR)/QueryExpression.query.res
	@ $(RM) $(TMP_DIR)/QueryExpression.query.res
	@ $(ECHO) +++ QueryExpression.query passed successfully +++

tests: QueryExpression.query.res 
                                                                                
poststarttests:

clean:
	$(RM) QueryExpression.query.res
                                                                              
install:




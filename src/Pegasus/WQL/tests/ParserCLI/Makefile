ROOT = ../../../../..

DIR = Pegasus/WQL/tests/ParserCLI

LIBRARIES = pegwql pegcommon

include $(ROOT)/mak/config.mak

PROGRAM = ParserCLI
SOURCES = ParserCLI.cpp

include $(ROOT)/mak/program.mak

tests: test1 test2 test3 test4 test5 test6

test1:
	@ ParserCLI "SELECT w,x,y,z FROM ClassName"

test2:
	@ ParserCLI "SELECT w,x,y,z FROM ClassName WHERE w = TRUE OR w = FALSE"

test3:
	@ ParserCLI "SELECT w,x,y,z FROM ClassName WHERE w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\"" 

test4:
	@ ParserCLI "SELECT * FROM ClassName WHERE (w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\") AND NOT w = TRUE IS NOT TRUE"

test5:
	@ ParserCLI "SELECT * FROM ClassName WHERE NOT NOT NOT x < 5"

test6:
	@ ParserCLI "SELECT * FROM ClassName WHERE v IS NULL"

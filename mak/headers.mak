
strip-headers:
	mu strip //% //% *.h *.cpp

#mu strip //BEGIN_LIC //END_HIST *.h *.cpp

prepend-headers:
	mu prepend $(ROOT)/doc/header.txt *.h *.cpp


strip-headers:
	mu strip //% //= *.h *.cpp

prepend-headers:
	mu prepend $(ROOT)/doc/license.txt *.h *.cpp


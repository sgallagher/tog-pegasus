
misc:

strip-license:
	mu strip //% //= *.h *.cpp

prepend-license:
	mu prepend $(ROOT)/doc/license.txt *.h *.cpp

zap:
	rm -f *.h *.cpp

# This is a 'standard' Makefile to build the library for the sample
# localized provider.  Note that DIRS is set to recurse to the msg
# sub-dir to build the resource bundles, and recurse.mak is included.

ROOT = ../../../..

DIR = Providers/sample/LocalizedProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleLocalizedProvider

LIBRARIES = \
	pegcommon \
	pegprovider
	
SOURCES = \
	LocalizedProviderMain.cpp \
	LocalizedProvider.cpp

include $(ROOT)/mak/library.mak

tests:




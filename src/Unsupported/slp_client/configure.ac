AC_PREREQ([2.53])
AC_REVISION([$Revision: 1.1.1.1 $])
AC_INIT([slp],[0.8] ,[lslp-bugs@lists.sourceforge.net], [lslp.tar.gz])

AC_COPYRIGHT([Configuration input files:
Copyright (c) 2002, Michael Day
Copyright (c) 2002, International Business Machines

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.])
AC_CONFIG_SRCDIR([config.guess])

AC_CANONICAL_BUILD



AC_PROG_LIBTOOL
## establish the kind of executable to build (debug or production)

if
[ [ $DEBUG ] ]
then
	echo Configuring to build $PACKAGE_NAME with debugging symbols
	AC_DEFINE([DEBUG], 1, [Enable debugging options in build.])

else
	echo Configuring to build a production version of $PACKAGE_NAME
fi

## libtool & Make variables
LIBTOOL=libtool
OBJ=.lo
IGNORE=-
LIB_PATH=-rpath
LEX=flex
YACC=bison
STATIC_LINK_CMD=-static
DLL_LINK_CMD=-module

MV=mv
MV_FLAGS=-fv
RM=rm
RM_FLAGS=-fv
CP=cp
CP_FLAGS=-fv
MAKE=make
## establish the target OS and PROC

case $build in
  *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
	AC_DEFINE([HOST_OS], ["Windows"], [Target Operation System])
	AC_DEFINE([OS_PORT_INCLUDE], ["lslp-windows.h"], [Portability Header])
	OS_PORT_INC=lslp-windows.h
	OS_PORT_SRC=lslp-windows.c
	OS_PORT_OBJ=lslp-windows.lo
	HOST_OS=Windows
	CC=cl
	LINKER=$CC
	EXE=.exe
	LIBS=ws2_32.lib
	THREAD_LIBS=
	REENTRANT_FLAG=
	NO_LINK_FLAG=
	LIB_SUFFIX=.lib
	DLL_SUFFIX=.dll
	EXE_OUT=/Fe
	OBJ_OUT=/Fo
	LIB_INCLUDE_FLAG
	AC_DEFINE([DLL_IMPORT], [__declspec(dllimport)], [DLL import declaration])
	AC_DEFINE([DLL_EXPORT], [__declspec(dllexport)], [DLL export declaration])
	if [ [ $DEBUG ] ] 
	then 
		CFLAGS="-GX -GR -W3 -Od -Zi -MDd -DDEBUG_ALLOC"
		LDFLAGS=-debug
	        DLL_LIB_FLAG=/LDd
		LFLAGS="-p -v -d"
		YFLAGS="-v -d"
        else
		CFLAGS="-GX -GR -W3 -O2 -MD"
		LDFLAGS=""
	        DLL_LIB_FLAG=/LD
		LFLAGS="-f -w"
		YFLAGS=-l
	fi 

	bindir=$SYSTEMROOT/system32
	sbindir=$SYSTEMROOT/system32
	libexecdir=$SYSTEMROOT/system32
	sysconfdir=$SYSTEMROOT/system32
	libdir=$SYSTEMROOT/system32


    ;;
  *-*-linux*)
	AC_DEFINE([HOST_OS], ["Linux"], [Target Operation System])
	AC_DEFINE([OS_PORT_INCLUDE], ["lslp-linux.h"], [Portability Header])
	OS_PORT_INC=lslp-linux.h
	OS_PORT_SRC=lslp-linux.c
	OS_PORT_OBJ=lslp-linux.lo
	HOST_OS=linux
	AC_PROG_CC()
	LINKER=$CC
	EXE=
	LIBS=
	LIB_INCLUDE_FLAG=-l
	THREAD_LIBS=-lpthread
	REENTRANT_FLAG=-D_REENTRANT
	NO_LINK_FLAG=-c
	LIB_SUFFIX=.o
	DLL_SUFFIX=.so
	AC_DEFINE([DLL_IMPORT], , [DLL import declaration])
	AC_DEFINE([DLL_EXPORT], , [DLL export declaration])
	EXE_OUT=-o
	OBJ_OUT=-o
	if [ [ $DEBUG ] ] 
	then 
		CFLAGS="-g -Wall -Dlinux -DDEBUG_ALLOC"
		LDFLAGS=-g 
		LFLAGS="-p -v -d "
		YFLAGS="-v -d"
        else
		CFLAGS="-O -Wall -Dlinux"
		LDFLAGS=-s 
		LFLAGS="-f -w"
		YFLAGS=-l
	fi 

	bindir=/bin
	sbindir=/sbin
	libexecdir=/lib
	sysconfdir=/etc
	libdir=/usr/lib

    ;;  
  *-*-aix*)
	AC_DEFINE([HOST_OS], ["AIX"], [Target Operation System])
	HOST_OS=AIX
    ;;
esac

case $build in 
  i?86-*-*)
	AC_DEFINE([PLATFORM], ["x86"], [Target Processor])
	PLATFORM=x86
    ;;
  ia64-*-*)
	AC_DEFINE([PLATFORM], ["ia64"], [Target Processor])
	PLATFORM=ia64
    ;;
  ppc-*-*)
	AC_DEFINE([PLATFORM], ["ppc"], [Target Processor])
	PLATFORM=ppc
    ;;
  mipsel-*-*)
	AC_DEFINE([PLATFORM], ["mipsel"], [Target Processor])
	PLATFORM=mipsel
  ;;
  mipseb-*-*)	
	AC_DEFINE([PLATFORM], ["mipseb"], [Target Processor])
	PLATFORM=mipseb
esac


## check for headers and declarations we need

# will be false on windows systems
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([linux/if.h], HAVE_IF_H=1, ,)

if
[ [ $HAVE_IF_H ] ]
then
	AC_DEFINE([HAVE_IF_H], 1, [Use linux include for IOCTL on network devices.])
fi

## create the appropriate directories to hold binaries
SRC_DIR=


## set up environment variables that control the build for specific variants 
case $HOST_OS in 
  linux)	
	case $PLATFORM in
            x86)

	    ;;

	    ia64)

	    ;;

            ppc)	


            ;;
        esac

  ;;
  Windows)	
	case $PLATFORM in
            x86)

	    ;;

	    ia64)

	    ;;

            ppc)	


            ;;
        esac

  ;;
  AIX)
	case $PLATFORM in
            x86)

	    ;;

	    ia64)

	    ;;

            ppc)	


            ;;
        esac
  ;;

esac

AC_C_BIGENDIAN 
AC_SUBST([WORDS_BIGENDIAN])
AC_SUBST([DEBUG])
AC_SUBST([PACKAGE_NAME])
AC_SUBST([PACKAGE_VERSION])
AC_SUBST([LIBTOOL])
AC_SUBST([OBJ])
AC_SUBST([IGNORE])
AC_SUBST([LIB_PATH])
AC_SUBST([LEX])
AC_SUBST([YACC])
AC_SUBST([STATIC_LINK_CMD])
AC_SUBST([DLL_LINK_CMD])
AC_SUBST([MV])
AC_SUBST([MV_FLAGS])
AC_SUBST([RM])
AC_SUBST([RM_FLAGS])
AC_SUBST([CP])
AC_SUBST([CP_FLAGS])
AC_SUBST([MAKE])
AC_SUBST([OS_PORT_INC])
AC_SUBST([OS_PORT_SRC])
AC_SUBST([OS_PORT_OBJ])
AC_SUBST([HOST_OS])
AC_SUBST([LINKER])
AC_SUBST([EXE])
AC_SUBST([LIBS])
AC_SUBST([THREAD_LIBS])
AC_SUBST([REENTRANT_FLAG])
AC_SUBST([NO_LINK_FLAG])
AC_SUBST([LIB_SUFFIX])
AC_SUBST([DLL_SUFFIX ])
AC_SUBST([EXE_OUT])
AC_SUBST([OBJ_OUT])
AC_SUBST([LIB_INCLUDE_FLAG])
AC_SUBST([CFLAGS])
AC_SUBST([LDFLAGS])
AC_SUBST([DLL_LIB_FLAG])
AC_SUBST([LFLAGS])
AC_SUBST([YFLAGS])
AC_SUBST([bindir])
AC_SUBST([sbindir])
AC_SUBST([libexecdir])
AC_SUBST([sysconfdir])
AC_SUBST([libdir])
AC_SUBST([PLATFORM])
AC_SUBST([CC])

echo Build Process set for $HOST_OS running on $PLATFORM	
echo Using Compiler: $CC $CFLAGS
echo Using Linker: $LINKER $LDFLAGS $LIBS
AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])

AC_OUTPUT

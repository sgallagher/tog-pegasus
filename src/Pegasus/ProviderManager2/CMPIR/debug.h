//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Frank Scheffler
//
// Modified By:  Adrian Schuur (schuur@de.ibm.com)
//               Marek Szermutzky, IBM (mszermutzky@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
  \file debug.h
  \brief Bug tracing facility.

  This header file defines macros and functions for tracing output
  using different debug levels, which can be defined during runtime as
  environment variable.

  Modules including this header file have to be compiled used -DDEBUG to
  enable debug support.

  \author Frank Scheffler
*/

#ifndef _REMOTE_CMPI_DEBUG_H
#define _REMOTE_CMPI_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#include <strings.h>
#endif

#define DEBUG_VERBOSE  3
#define DEBUG_INFO     2
#define DEBUG_NORMAL   1
#define DEBUG_CRITICAL 0



#define TRACE(level,args) \
        if ( __trace_level ( (level) ) ) { \
                char * __msg = __trace_format args; \
                __trace_this ( (level), \
                               __FILE__, __FUNCTION__, __LINE__, \
                               __msg ); \
        }


#if defined PEGASUS_DEBUG
#define TRACE_VERBOSE(args)  TRACE(DEBUG_VERBOSE,args)
#else
#define TRACE_VERBOSE(args)
#endif

#if defined PEGASUS_DEBUG
#define TRACE_INFO(args)     TRACE(DEBUG_INFO,args)
#else
#define TRACE_INFO(args)
#endif

#if defined PEGASUS_DEBUG
#define TRACE_NORMAL(args)   TRACE(DEBUG_NORMAL,args)
#else
#define TRACE_NORMAL(args)
#endif

#if defined PEGASUS_DEBUG
#define TRACE_CRITICAL(args) TRACE(DEBUG_CRITICAL,args)
#else
#define TRACE_CRITICAL(args)
#endif

#if defined PEGASUS_DEBUG
#define START_DEBUGGER __start_debugger ()
#else
#define START_DEBUGGER
#endif


/****************************************************************************/

#if defined PEGASUS_DEBUG

#ifdef __GNUC__
static int __trace_level ( int ) __attribute__ ((unused));

static char * __trace_format ( char *, ... )
     __attribute__ ((unused, format (printf, 1, 2)));

static void __trace_this ( int, const char *, const char *, int, char * )
     __attribute__ ((unused));

static void __start_debugger () __attribute__ ((unused));
#endif

/****************************************************************************/

static char * __debug_levels[] = {
	"critical", "normal", "info", "verbose"
};

/****************************************************************************/

static int __trace_level ( int level )
{
	char * l = getenv ( "RCMPI_DEBUG" );
	int i = sizeof ( __debug_levels ) / sizeof ( char * );

	if ( l == NULL ) return 0;

	while ( i-- )
		if ( strcasecmp ( l, __debug_levels[i] ) == 0 )
			return ( level <= i );

	return 0;
}


static char * __trace_format ( char * fmt, ... )
{
	va_list ap;
	char * msg = (char *) malloc ( 512 );

	va_start ( ap, fmt );
#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	vsprintf ( msg, fmt, ap );
#else
	vsnprintf ( msg, 512, fmt, ap );
#endif
	return msg;
}


static void __trace_this ( int level,
			   const char * file,
			   const char * function,
			   int line,
			   char * msg )
{
	fprintf ( stderr,
		  "--rcmpi(%s)--[%d(%d,%d)]:%s:%s(%d): %s\n",
		   __debug_levels[level],
		  getpid (), getuid (), getgid (),
		  file, function, line,
		  msg );
	free ( msg ); \
}


static void __start_debugger ()
{
	int ch;
	char * debugger = getenv ( "RCMPI_DEBUGGER" );

	if ( debugger != NULL ) {

		if ( ( ch = fork () ) ) {

			sleep ( 20 ); // wait until debugger traces us

		} else {

			char pid[10];
			char * argv[] = { debugger,
					  "OOP-Provider",
					  pid,
					  NULL };

			sprintf ( pid, "%d", getppid () );

			execv ( debugger, argv );

			TRACE_CRITICAL(("could not start debugger \"%s\", "
					"check RCMPI_DEBUGGER environment "
					"variable.",
					debugger));
			exit ( -1 );
		}
	}
}


#endif
#ifndef PEGASUS_PLATFORM_LINUX_GENERIC_GNU 
static void error_at_line( int a_num, int error, char* filename, int line, char* message, ... )
{
   va_list ap;
   char * msg = (char *) malloc ( 512 );

   va_start ( ap, message );
   vsprintf ( msg/*, 512*/, message, ap );

   fprintf(stderr, "Error in line %d of file %s: %s - %s\n", line, filename, strerror(error), msg);

   free (msg);

   if (a_num < 0)
   {
      exit(a_num);
   }
}
#endif

#endif


/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/

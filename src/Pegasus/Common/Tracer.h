//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Tracer_h
#define Pegasus_Tracer_h

#include <stdarg.h>
#include <fstream>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/TraceComponents.h>
#include <Pegasus/Common/TraceFileHandler.h>


PEGASUS_NAMESPACE_BEGIN

/** Tracer implements tracing of messages to a defined file
 */

class PEGASUS_COMMON_LINKAGE Tracer
{
public:

    /** Levels of trace
        Trace messages are written to the trace file only if they are at or
        above a given trace level
        LEVEL1 - Function Entry/Exit
        LEVEL2 - Basic flow trace messages, low data detail
        LEVEL3 - Inter-function logic flow, medium data detail
        LEVEL4 - High data detail
     */
    static const Uint32 LEVEL1;
    static const Uint32 LEVEL2;
    static const Uint32 LEVEL3;
    static const Uint32 LEVEL4;
    
    // PEGASUS_REMOVE_TRACE defines the compile time inclusion of the Trace 
    // interfaces. If defined the interfaces map to empty functions
    
    #ifdef PEGASUS_REMOVE_TRACE
        
        inline static void trace(
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char *fmt,
            ...)
        {
            // empty function
        }
        
        inline static void trace(
            const char*  fileName,
            const Uint32 lineNum,
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char* fmt,
            ...)
        {
            // empty function
        }

    #else
        
        /** Traces the given message 
            @param    traceComponent  component being traced
            @param    traceLevel      trace level of the trace message
            @param    *fmt            printf style format string
            @param    ...             variable argument list
         */
        inline static void trace(
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char *fmt,
            ...)
        {
            va_list argList;
        
            va_start(argList,fmt);
            _trace(traceComponent,traceLevel,fmt,argList); 
            va_end(argList);
        }
        
        /** Traces the given message. Overloaded to include the filename and 
	    the line number of trace origin. 
            @param    fileName        filename of the trace originator
            @param    lineNum         line number of the trace originator
            @param    traceComponent  component being traced
            @param    traceLevel      trace level of the trace message
            @param    *fmt            printf style format string
            @param    ...             variable argument list
         */
        inline static void trace(
            const char* fileName,
            const Uint32 lineNum,
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char* fmt,
            ...)
        {
            va_list argList;
        
            va_start(argList,fmt);
            _trace(fileName,lineNum,traceComponent,traceLevel,fmt,argList); 
            va_end(argList);
        }
        
    #endif 

    // End of PEGASUS_REMOVE_TRACE
        
    /** Traces method entry. 
        @param    fileName        filename of the trace originator
        @param    lineNum         line number of the trace originator
        @param    traceComponent  component being traced
        @param    methodName      method being traced
     */
    inline static void traceEnter(
        const char* fileName,
        const Uint32 lineNum,
        const Uint32 traceComponent,
        const char* methodName)
    {
        _traceEnter( fileName, lineNum, traceComponent, "%s %s",
    	    _FUNC_ENTER_MSG, methodName);
    }
    
    /** Traces method exit. 
        @param    fileName        filename of the trace originator
        @param    lineNum         line number of the trace originator
        @param    traceComponent  component being traced
        @param    methodName      method being traced
     */
    inline static void traceExit(
        const char* fileName,
        const Uint32 lineNum,
        const Uint32 traceComponent,
        const char* methodName)
    {
        _traceExit( fileName, lineNum, traceComponent, "%s %s",
    	    _FUNC_EXIT_MSG, methodName);
    }

    /** Set the trace file to the given file
        @param    traceFile       full path of the trace file
        @return   0               if the filepath is valid 
                  1               if an error occurs while opening the file in
                                  append mode
     */
    static Uint32 setTraceFile(const char* traceFile);

    /** Set the trace level to the given level
        @param    traceLevel      trace level to be set
        @return   0               if trace level is valid
                  1               if trace level is invalid
     */
    static Uint32 setTraceLevel(const Uint32 traceLevel);

   /** Set components to be traced
       @param    traceComponents list of components to be traced, components 
                 should be separated by ','
    */
    static void setTraceComponents(String traceComponents);

private:

    static const char   _COMPONENT_SEPARATOR;
    static const Uint32 _NUM_COMPONENTS;
    static const Uint32 _STRLEN_MAX_UNSIGNED_INT;
    Boolean*            _traceComponentMask;
    Uint32              _traceLevelMask;
    TraceFileHandler*   _traceHandler;
    static Tracer*      _tracerInstance;

    // Message Strings for fucntion Entry and Exit
    static const char _FUNC_ENTER_MSG[]; 
    static const char _FUNC_EXIT_MSG[]; 

    // Message Strings for Logger
    static const char _LOG_MSG1[]; 
    static const char _LOG_MSG2[]; 

    // Checks if trace is enabled for the given component and trace level
    // @param    traceComponent  component being traced
    // @param    traceLevel      level of the trace message
    // @return   0               if the component and level are not enabled 
    //           1               if the component and level are enabled 
    static Boolean _isTraceEnabled(
	const Uint32 traceComponent,
	const Uint32 traceLevel);

    // Traces the given message 
    //  @param    traceComponent  component being traced
    //  @param    traceLevel      level of the trace message
    //  @param    *fmt            printf style format string
    //  @param    argList         variable argument list
    static void _trace(
	const Uint32 traceComponent,
        const Uint32 traceLevel,
	const char* fmt,
	va_list argList);

    // Traces the given message. Overloaded to include the file name and the
    // line number as one of the parameters.
    // @param    traceComponent  component being traced
    // @param    traceLevel      level of the trace message
    // @param    message         message header (file name:line number)
    // @param    *fmt            printf style format string
    // @param    argList         variable argument list
    static void _trace(
	const char* fileName,
	const Uint32 lineNum,
	const Uint32 traceComponent,
        const Uint32 traceLevel,
	const char* fmt,
	va_list argList);

    // Called by all the trace interfaces to log message to the 
    // trace file
    // @param    fileName        filename of the trace originator
    // @param    lineNum         line number of the trace originator
    // @param    traceComponent  component being traced
    // @param    *fmt            printf style format string
    // @param    argList         variable argument list
    static void _trace(
	const Uint32 traceComponent,
	const char* message,
	const char* fmt,
	va_list argList);

    // Traces method enter
    // @param    fileName        filename of the trace originator
    // @param    lineNum         line number of the trace originator
    // @param    traceComponent  component being traced
    // @param    *fmt            printf style format string
    // @param    ...             variable argument list
    static void _traceEnter(
	const char* fileName,
	const Uint32 lineNum,
	const Uint32 traceComponent,
	const char* fmt,
	...);

    // Traces method exit
    // @param    fileName        filename of the trace originator
    // @param    traceComponent  component being traced
    // @param    *fmt            printf style format string
    // @param    ...             variable argument list
    static void _traceExit(
	const char* fileName,
	const Uint32 lineNum,
	const Uint32 traceComponent,
	const char* fmt,
	...);

    // Tracer constructor
    // Constructor is private to prevent construction of Tracer objects
    // Single Instance of Tracer is maintained for each process.
    Tracer();

    //   Tracer destructor
    ~Tracer();

    // Returns the Singleton instance of the Tracer
    // @return   Tracer*  Instance of Tracer 
    static Tracer* _getInstance();
};

// Define the macros for method entry/exit
#ifdef PEGASUS_REMOVE_TRACE
    #define PEG_FUNC_ENTER(traceComponent,methodName) 
    #define PEG_FUNC_EXIT(traceComponent,methodName) 
#else
    /** Macro for tracing method entry
        @param    traceComponent  component being traced
        @param    methodName      name of the method
     */
    #define PEG_FUNC_ENTER(traceComponent,methodName) \
	Tracer::traceEnter(__FILE__, __LINE__,traceComponent,methodName)

    /** Macro for tracing method exit
        @param    traceComponent  component being traced
        @param    methodName      name of the method
     */
    #define PEG_FUNC_EXIT(traceComponent,methodName) \
	Tracer::traceExit(__FILE__,__LINE__,traceComponent,methodName)
#endif

PEGASUS_NAMESPACE_END
#endif /* Pegasus_Tracer_h */

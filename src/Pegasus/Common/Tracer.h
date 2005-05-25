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
// Author: Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Tracer_h
#define Pegasus_Tracer_h

#include <stdarg.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/TraceComponents.h>
#include <Pegasus/Common/TraceFileHandler.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>

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
        
        inline static void traceBuffer(
            const char*  fileName,
            const Uint32 lineNum,
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char*  data,
            const Uint32 size)
        {
            // empty function
        }
        inline static void traceBuffer(
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char* data,
            const Uint32 size)
        {
            // empty function
        }
        
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

        inline static void trace(
            const char*   fileName,
            const Uint32  lineNum,
            const Uint32  traceComponent,
            const Uint32  traceLevel,
            const String& traceString)
        {
            // empty function
        }

        inline static void traceCIMException(
            const Uint32  traceComponent,
            const Uint32  traceLevel,
            CIMException  cimException)
        {
            // empty function
        }

        static Uint32 setTraceFile(const char* traceFile)
	{
	    // empty function
	    return 0;
        }

        static Uint32 setTraceLevel(const Uint32 traceLevel)
	{
	    // empty function
	    return 0;
        }

        static void setTraceComponents(
	  const String& traceComponents)
	{
	      // empty function
        }
        

    #else
        /** Traces the specified number of bytes in a given buffer 
            @param    traceComponent  component being traced
            @param    traceLevel      trace level of the trace message
            @param    data            buffer to be traced
            @param    size            number of bytes to be traced 
         */
        inline static void traceBuffer(
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char*  data,
            const Uint32 size)
        {
	    _traceBuffer( traceComponent, traceLevel, data, size );
        }
        
        /** Traces the specified number of bytes in a given buffer 
	    Overloaded to include the filename and the line number 
	    of trace origin. 
            @param    fileName        filename of the trace originator
            @param    lineNum         line number of the trace originator
            @param    traceComponent  component being traced
            @param    traceLevel      trace level of the trace message
            @param    data            buffer to be traced
            @param    size            size of the buffer
         */
        inline static void traceBuffer(
            const char*  fileName,
            const Uint32 lineNum,
            const Uint32 traceComponent,
            const Uint32 traceLevel,
            const char*  data,
            const Uint32 size)
        {
            _traceBuffer( fileName, lineNum,
		          traceComponent, traceLevel, data, size ); 
        }
        
        
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

        /** Traces the given string.  Overloaded to include the filename
            and line number of trace origin.
            @param    fileName        filename of the trace originator
            @param    lineNum         line number of the trace originator
            @param    traceComponent  component being traced
            @param    traceLevel      trace level of the trace message
            @param    traceString     the string to be traced
         */
        inline static void trace(
            const char*   fileName,
            const Uint32  lineNum,
            const Uint32  traceComponent,
            const Uint32  traceLevel,
            const String& traceString)
        {
            _traceString( fileName, lineNum, traceComponent, traceLevel,
		          traceString ); 
        }

        /** Traces the message in the given CIMException object.  The message
            written to the trace file will include the source filename and
            line number of the CIMException originator.
            @param    traceComponent  component being traced
            @param    traceLevel      trace level of the trace message
            @param    CIMException    the CIMException to be traced.
         */
        inline static void traceCIMException(
            const Uint32  traceComponent,
            const Uint32  traceLevel,
            CIMException  cimException)
        {
            _traceCIMException( traceComponent, traceLevel, cimException );
        }

        /** Set the trace file to the given file
            @param    traceFile       full path of the trace file
            @return   0               if the filepath is valid 
                      1               if the traceFile is an empty string or
                                      if an error occurs while opening the file
				      in append mode
        */
        static Uint32 setTraceFile(const char* traceFile);

        /** Set the trace level to the given level
            @param    traceLevel      trace level to be set
            @return   0               if trace level is valid
                      1               if trace level is invalid
        */
        static Uint32 setTraceLevel(const Uint32 traceLevel);

        /** Set components to be traced
            @param    traceComponents list of components to be traced, 
		      components should be separated by ','
        */
        static void setTraceComponents(
	   const String& traceComponents);
        
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
    	    _METHOD_ENTER_MSG, methodName);
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
    	    _METHOD_EXIT_MSG, methodName);
    }

    /** Validates the File Path for the trace File
        @param    filePath full path of the file 
        @return   1        if the file path is valid
                  0        if the file path is invalid
     */
    static Boolean isValidFileName(const char* filePath);

    /** Validates the trace components
        @param    traceComponents   comma separated list of trace components
        @return   1        if the components are valid
                  0        if one or more components are invalid
     */
    static Boolean isValidComponents(const String& traceComponents);

    /** Validates the trace components
        @param    traceComponents   comma separated list of trace components
        @param    invalidComponents comma separated list of invalid components
        @return   1        if the components are valid
                  0        if one or more components are invalid
     */
    static Boolean isValidComponents(
        const String& traceComponents,
        String& invalidComponents);

    /** Specify the name of the module being traced.  If non-empty, this
        value is used as an extension to the name of the trace file.
        @param    moduleName   Name of the module being traced.
     */
    static void setModuleName(const String& moduleName);

private:

    static const char   _COMPONENT_SEPARATOR;
    static const Uint32 _NUM_COMPONENTS;
    static const Uint32 _STRLEN_MAX_UNSIGNED_INT;
    static const Uint32 _STRLEN_MAX_PID_TID;
    static const Boolean _SUCCESS;
    static const Boolean _FAILURE;
    AutoArrayPtr<Boolean> _traceComponentMask;
    Uint32              _traceLevelMask;
    AutoPtr<TraceFileHandler> _traceHandler;
    String              _moduleName;
    static Tracer*      _tracerInstance;

    // Message Strings for function Entry and Exit
    static const char _METHOD_ENTER_MSG[]; 
    static const char _METHOD_EXIT_MSG[]; 

    // Message Strings for Logger
    static const char _LOG_MSG[]; 

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

    //  Traces the specified number of bytes in a given buffer
    //  @param    traceComponent  component being traced
    //  @param    traceLevel      trace level of the trace message
    //  @param    data            buffer to be traced
    //  @param    size            number of bytes to be traced
    static void _traceBuffer(
	const Uint32 traceComponent,
        const Uint32 traceLevel,
        const char*  data,
        const Uint32 size);

    //  Traces the specified number of bytes in a given buffer
    //  Overloaded to include the filename and the line number
    //  of trace origin.
    //  @param    fileName        filename of the trace originator
    //  @param    lineNum         line number of the trace originator
    //  @param    traceComponent  component being traced
    //  @param    traceLevel      trace level of the trace message
    //  @param    data            buffer to be traced
    //  @param    size            size of the buffer
    static void _traceBuffer(
	const char*  fileName,
	const Uint32 lineNum,
	const Uint32 traceComponent,
        const Uint32 traceLevel,
        const char*  data,
        const Uint32 size);

    //  Traces the given string.
    //  @param    traceComponent  component being traced
    //  @param    traceLevel      trace level of the trace message
    //  @param    traceString     the string to be traced
    //
    static void _traceString(
        const Uint32  traceComponent,
        const Uint32  traceLevel,
        const String& traceString);

    //  Traces a given string.  Overloaded to include the filename
    //  and line number of trace origin.
    //  @param    fileName        filename of the trace originator
    //  @param    lineNum         line number of the trace originator
    //  @param    traceComponent  component being traced
    //  @param    traceLevel      trace level of the trace message
    //  @param    traceString     the string to be traced
    //
    static void _traceString(
        const char*   fileName,
        const Uint32  lineNum,
        const Uint32  traceComponent,
        const Uint32  traceLevel,
        const String& traceString);

    //  Traces the message in the given CIMException object.  The message
    //  to be written to the trace file will include the source filename and 
    //  line number of the CIMException originator.
    //  @param    traceComponent  component being traced
    //  @param    traceLevel      trace level of the trace message
    //  @param    CIMException    the CIMException to be traced.
    //
    static void _traceCIMException(
        const Uint32  traceComponent,
        const Uint32  traceLevel,
        CIMException  cimException);
        
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

// Define the macros for method entry/exit, and tracing a given string
#ifdef PEGASUS_REMOVE_TRACE
    #define PEG_METHOD_ENTER(traceComponent,methodName) 
    #define PEG_METHOD_EXIT() 
    #define PEG_TRACE_STRING(traceComponent,traceLevel,traceString) 
#else
    /** Macro for tracing method entry
        @param    traceComponent  component being traced
        @param    methodName      name of the method
     */
    #define PEG_METHOD_ENTER(traceComponent,methodName) \
	const char *PEG_METHOD_NAME = methodName; \
	const Uint32 PEG_TRACE_COMPONENT = traceComponent; \
	Tracer::traceEnter(__FILE__,__LINE__,PEG_TRACE_COMPONENT,PEG_METHOD_NAME)

    /** Macro for tracing method exit
     */
    #define PEG_METHOD_EXIT() \
	Tracer::traceExit(__FILE__,__LINE__,PEG_TRACE_COMPONENT,PEG_METHOD_NAME)

    /** Macro for tracing a string
        @param    traceComponent  component being traced
        @param    traceLevel      trace level of the trace message
        @param    traceString     the string to be traced
     */
    #define PEG_TRACE_STRING(traceComponent,traceLevel,traceString) \
	Tracer::trace(__FILE__, __LINE__,traceComponent,traceLevel,traceString)

#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Tracer_h */

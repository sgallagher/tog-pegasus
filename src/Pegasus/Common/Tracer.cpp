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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/System.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// Set the trace levels
// These levels will be compared against a trace level mask to determine
// if a specific trace level is enabled 

const Uint32 Tracer::LEVEL1 = (1 << 0);
const Uint32 Tracer::LEVEL2 = (1 << 1);
const Uint32 Tracer::LEVEL3 = (1 << 2);
const Uint32 Tracer::LEVEL4 = (1 << 3);

// Set the return codes
const Boolean Tracer::_SUCCESS = 1;
const Boolean Tracer::_FAILURE = 0;
String  Tracer::_EMPTY_STRING = String::EMPTY;

// Set the Enter and Exit messages
const char Tracer::_FUNC_ENTER_MSG[] = "Entering method";
const char Tracer::_FUNC_EXIT_MSG[]  = "Exiting method";

// Set Log messages
const char Tracer::_LOG_MSG1[] = "LEVEL1 not enabled with Tracer::trace call.";
const char 
      Tracer::_LOG_MSG2[] = "LEVEL1 not enabled with Tracer::traceBuffer call.";
const char Tracer::_LOG_MSG3[]="Use trace macros, PEG_FUNC_ENTER/PEG_FUNC_EXIT";

// Initialize singleton instance of Tracer
Tracer* Tracer::_tracerInstance = 0;
 
// Set component separator
const char Tracer::_COMPONENT_SEPARATOR = ',';

// Set the number of defined components
const Uint32 Tracer::_NUM_COMPONENTS = 
    sizeof(TRACE_COMPONENT_LIST)/sizeof(TRACE_COMPONENT_LIST[0]);

// Set the line maximum
const Uint32 Tracer::_STRLEN_MAX_UNSIGNED_INT = 21;

// Set the max PID and Thread ID Length
const Uint32 Tracer::_STRLEN_MAX_PID_TID = 20;

////////////////////////////////////////////////////////////////////////////////
// Tracer constructor
// Constructor is private to preclude construction of Tracer objects
// Single Instance of Tracer is maintained for each process.
////////////////////////////////////////////////////////////////////////////////
Tracer::Tracer()
{
    // Initialize Trace File Handler
    _traceHandler=new TraceFileHandler();
    _traceLevelMask=0;
    _traceComponentMask=new Boolean[_NUM_COMPONENTS];

    // Initialize ComponentMask array to false
    for (Uint32 index=0;index < _NUM_COMPONENTS; 
	_traceComponentMask[index++]=false);
}
            
////////////////////////////////////////////////////////////////////////////////
//Tracer destructor
////////////////////////////////////////////////////////////////////////////////
Tracer::~Tracer()
{
    delete []_traceComponentMask;
    delete _traceHandler;
    delete _tracerInstance;
}


////////////////////////////////////////////////////////////////////////////////
//Traces the given message
////////////////////////////////////////////////////////////////////////////////
void Tracer::_trace(
    const Uint32 traceComponent,
    const Uint32 traceLevel,
    const char* fmt,
    va_list argList)
{
    if ( traceLevel == LEVEL1 )
    {
	// ATTN: Setting the Log file type to DEBUG_LOG
	// May need to change to an appropriate log file type
	Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,"$0 $1",
           _LOG_MSG1,_LOG_MSG3); 
    }
    else
    {
        if (_isTraceEnabled(traceComponent,traceLevel))
        {
            _trace(traceComponent,"",fmt,argList); 
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
//Traces the given message - Overloaded for including FileName and Line number
////////////////////////////////////////////////////////////////////////////////
void Tracer::_trace(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const Uint32 traceLevel,
    const char* fmt,
    va_list argList)
{
    char* message;

    if ( traceLevel == LEVEL1 )
    {
	Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,"$0 $1",
           _LOG_MSG1,_LOG_MSG3); 
    }
    else
    {
        if (_isTraceEnabled(traceComponent,traceLevel))
        {
            //
            // Allocate memory for the message string
            // Needs to be updated if additional info is added
            //
	    message = new char[ strlen(fileName) + 
		_STRLEN_MAX_UNSIGNED_INT + _STRLEN_MAX_PID_TID ];
            sprintf(
               message,
               "[%d:%d:%s:%d]: ",
               System::getPID(),
               pegasus_thread_self(),
               fileName,
               lineNum);

            _trace(traceComponent,message,fmt,argList); 
	    delete []message;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//Traces the given buffer
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceBuffer(
    const Uint32 traceComponent,
    const Uint32 traceLevel,
    const char*  data,
    const Uint32 size)
{
    if ( traceLevel == LEVEL1 )
    {
        Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,"$0 $1",
           _LOG_MSG2,_LOG_MSG3);
    }
    else
    {
        if (_isTraceEnabled(traceComponent,traceLevel))
        {
            char* tmpBuf = new char[size+1];

            strncpy( tmpBuf, data, size );
            tmpBuf[size] = '\0';
            trace(traceComponent,traceLevel,"%s",tmpBuf);

            delete []tmpBuf;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
//Traces the given buffer - Overloaded for including FileName and Line number
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceBuffer(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const Uint32 traceLevel,
    const char*  data,
    const Uint32 size)
{
    if ( traceLevel == LEVEL1 )
    {
        Logger::put(Logger::DEBUG_LOG,"Tracer",Logger::WARNING,"$0 $1",
           _LOG_MSG2,_LOG_MSG3);
    }
    else
    {
        if ( _isTraceEnabled( traceComponent, traceLevel ) )
        {
            char* tmpBuf = new char[size+1];

            strncpy( tmpBuf, data, size );
            tmpBuf[size] = '\0';
            trace(fileName,lineNum,traceComponent,traceLevel,"%s",tmpBuf);

            delete []tmpBuf;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//Traces method entry
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceEnter(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const char* fmt,
    ...)
{
    va_list argList;
    char* message;

    if (_isTraceEnabled(traceComponent,LEVEL1))
    {

        va_start(argList,fmt);

        //
        // Allocate memory for the message string
        // Needs to be updated if additional info is added
        //
	message = new char[ strlen(fileName) + 
	    _STRLEN_MAX_UNSIGNED_INT + _STRLEN_MAX_PID_TID ];
        sprintf(
           message,
           "[%d:%d:%s:%d]: ",
           System::getPID(),
           pegasus_thread_self(),
           fileName,
           lineNum);
        _trace(traceComponent,message,fmt,argList); 

        va_end(argList);
        delete []message;
    }
}

////////////////////////////////////////////////////////////////////////////////
//Traces method exit
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceExit(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const char* fmt
    ...)
{
    va_list argList;
    char* message;

    if (_isTraceEnabled(traceComponent,LEVEL1))
    {
        va_start(argList,fmt);
 
        //
        // Allocate memory for the message string
        // Needs to be updated if additional info is added
        //
	message = new char[ strlen(fileName) + 
	    _STRLEN_MAX_UNSIGNED_INT + _STRLEN_MAX_PID_TID ];
        sprintf(
           message,
           "[%d:%d:%s:%d]: ",
           System::getPID(),
           pegasus_thread_self(),
           fileName,
           lineNum);
        _trace(traceComponent,message,fmt,argList); 
        va_end(argList);

        delete []message;
    }
}

////////////////////////////////////////////////////////////////////////////////
//Checks if trace is enabled for the given component and level
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::_isTraceEnabled(const Uint32 traceComponent,
    const Uint32 traceLevel)
{
    Tracer* instance = _getInstance();
    if (traceComponent >= _NUM_COMPONENTS)
    {
	return false;
    }
    return ((instance->_traceComponentMask[traceComponent]) &&
	    (traceLevel  & instance->_traceLevelMask));
}

////////////////////////////////////////////////////////////////////////////////
//Called by all trace interfaces to log message to trace file
////////////////////////////////////////////////////////////////////////////////
void Tracer::_trace(
    const Uint32 traceComponent,
    const char* message,
    const char* fmt,
    va_list argList)
{
    char* msgHeader;

    // Get the current system time and prepend to message
    String currentTime = System::getCurrentASCIITime();
    ArrayDestroyer<char> timeStamp(currentTime.allocateCString());

    //
    // Allocate messageHeader. 
    // Needs to be updated if additional info is added
    //
    msgHeader = new char [strlen(message)
        + strlen(TRACE_COMPONENT_LIST[traceComponent]) 
	+ strlen(timeStamp.getPointer()) + _STRLEN_MAX_PID_TID];

    // Construct the message header
    // The message header is in the following format 
    // timestamp: <component name> [file name:line number]
    if (strcmp(message,"") != 0)
    {
        sprintf(msgHeader,"%s: %s %s",timeStamp.getPointer(),
            TRACE_COMPONENT_LIST[traceComponent] ,message);
    }
    else
    {
        //
        // Since the message is blank form a string using the pid and tid
        //
        char*  tmpBuffer;

        //
        // Allocate messageHeader. 
        // Needs to be updated if additional info is added
        //
	tmpBuffer = new char[_STRLEN_MAX_PID_TID];
        sprintf(tmpBuffer,"[%d:%d]: ",System::getPID(),pegasus_thread_self());

        sprintf(msgHeader,"%s: %s %s ",timeStamp.getPointer(),
            TRACE_COMPONENT_LIST[traceComponent] ,tmpBuffer );
        delete []tmpBuffer;
    }

    // Call trace file handler to write message
    _getInstance()->_traceHandler->handleMessage(msgHeader,fmt,argList);
    delete []msgHeader;
}

////////////////////////////////////////////////////////////////////////////////
//Validate the trace file
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::isValid(const char* filePath)
{
    return (_getInstance()->_traceHandler->isValidFilePath(filePath));
}

////////////////////////////////////////////////////////////////////////////////
//Validate the trace components
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::isValid(
		     const String traceComponents, String& invalidComponents)
{
    // Validate the trace components and modify the traceComponents argument
    // to reflect the invalid components

    Uint32    position=0;
    Uint32    index=0;
    String    componentName = String::EMPTY;
    String    componentStr = String::EMPTY;
    Boolean   validComponent=false;
    Boolean   retCode=true;

    componentStr = traceComponents;
    invalidComponents = String::EMPTY;

    if (componentStr != String::EMPTY)
    {
        // Check if ALL is specified
        if (String::equalNoCase(componentStr,"ALL"))
        {
            return _SUCCESS;
        }

        // Append _COMPONENT_SEPARATOR to the end of the traceComponents
        componentStr += _COMPONENT_SEPARATOR;

        while (componentStr != String::EMPTY)
        {
	    //
            // Get the Component name from traceComponents.
            // Components are separated by _COMPONENT_SEPARATOR
	    //
            position = componentStr.find(_COMPONENT_SEPARATOR);
            componentName = componentStr.subString(0,(position));

            // Lookup the index for Component name in TRACE_COMPONENT_LIST
            index = 0;
            validComponent = false;

            while (index < _NUM_COMPONENTS)
            {
                if (String::equalNoCase( 
		       componentName, TRACE_COMPONENT_LIST[index]))
                {
                    // Found component, break from the loop
		    validComponent = true;
                    break;
                }
                else
                {
                   index++;
                }
            }

            // Remove the searched componentname from the traceComponents
            componentStr.remove(0,position+1);

	    if ( !validComponent )
	    {
		invalidComponents += componentName;
		invalidComponents += _COMPONENT_SEPARATOR;
            }
        }
    }
    else
    {
	// trace components is empty, it is a valid value so return true
	return _SUCCESS;
    }
    if ( invalidComponents != String::EMPTY )
    {
	retCode = false;
	//
	// Remove the extra ',' at the end
	//
	invalidComponents.remove(
	    invalidComponents.reverseFind(_COMPONENT_SEPARATOR));
    }
    return retCode;
}
    
////////////////////////////////////////////////////////////////////////////////
//Returns the Singleton instance of the Tracer
////////////////////////////////////////////////////////////////////////////////
Tracer* Tracer::_getInstance()
{
    if (_tracerInstance == 0)
    {
        _tracerInstance = new Tracer();
    }
    return _tracerInstance;
}

// PEGASUS_REMOVE_TRACE defines the compile time inclusion of the Trace 
// interfaces. If defined the interfaces map to empty functions

#ifndef PEGASUS_REMOVE_TRACE

////////////////////////////////////////////////////////////////////////////////
//Set the trace file
////////////////////////////////////////////////////////////////////////////////
Uint32 Tracer::setTraceFile(const char* traceFile)
{
    return (_getInstance()->_traceHandler->setFileName (traceFile));
} 

////////////////////////////////////////////////////////////////////////////////
//Set the trace level
////////////////////////////////////////////////////////////////////////////////
Uint32 Tracer::setTraceLevel(const Uint32 traceLevel)
{
    Uint32 retCode = 0;

    switch (traceLevel)
    {
	case LEVEL1:
            _getInstance()->_traceLevelMask = 0x01;
	    break;
    
        case LEVEL2:
            _getInstance()->_traceLevelMask = 0x03;
	    break;

        case LEVEL3:
            _getInstance()->_traceLevelMask = 0x07;
	    break;

        case LEVEL4:
            _getInstance()->_traceLevelMask = 0x0F;
	    break;

        default:
            _getInstance()->_traceLevelMask = 0;
            retCode = 1;
    }
    return retCode;
}

////////////////////////////////////////////////////////////////////////////////
// Set components to be traced. 
////////////////////////////////////////////////////////////////////////////////
void Tracer::setTraceComponents( const String traceComponents )
{
    Uint32 position          = 0;
    Uint32 index             = 0;
    String componentName     = String::EMPTY;
    String componentStr      = traceComponents;
    String invalidComponents = String::EMPTY;
    
    if (componentStr != String::EMPTY)
    {
        // Check if ALL is specified
        if (String::equalNoCase(componentStr,"ALL"))
        {
            for (index=0; index < _NUM_COMPONENTS;
                    _getInstance()->_traceComponentMask[index++] = true);
            return ; 
        }

        // initialise ComponentMask array to False
        for (index = 0;index < _NUM_COMPONENTS; 
	        _getInstance()->_traceComponentMask[index++] = false);

 	// Append _COMPONENT_SEPARATOR to the end of the traceComponents
        componentStr += _COMPONENT_SEPARATOR;

        while (componentStr != String::EMPTY)
        {
            // Get the Component name from traceComponents. 
	    // Components are separated by _COMPONENT_SEPARATOR
            position = componentStr.find(_COMPONENT_SEPARATOR);
 	    componentName = componentStr.subString(0,(position));

	    // Lookup the index for Component name in TRACE_COMPONENT_LIST
            index = 0;
	    while (index < _NUM_COMPONENTS)
	    {
	        if (String::equalNoCase(
		       componentName,TRACE_COMPONENT_LIST[index]))
	        {
                    _getInstance()->_traceComponentMask[index]=true;

                    // Found component, break from the loop
                    break;
	        }
	        else
	        {
	 	    index++;
                }
            }   

            // Remove the searched componentname from the traceComponents
            componentStr.remove(0,position+1);
        }
    }
    else
    {
        // initialise ComponentMask array to False
        for (Uint32 index = 0;index < _NUM_COMPONENTS; 
                 _getInstance()->_traceComponentMask[index++] = false);
    }
    return ;
}

#endif

PEGASUS_NAMESPACE_END

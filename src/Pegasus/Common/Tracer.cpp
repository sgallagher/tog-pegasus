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

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Destroyer.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// ATTN  mdday@us.ibm.com Wed Aug  1 10:31:51 2001
/// el-cheapo version of ltoa
// ltoa is implemented differently from platform to platform
// Linux does not have it at all.
// windows implements it as char * _ltoa(long, char *, int); 
//
// Because of the static buffer this you should copy the 
// return value before another thread clobbers it. 
// If using for temporary values, as in :
// strlen(ltoa(linenumber)) 
// it should be safe enough for trace functions. . 
#ifndef PEGASUS_PLATFORM_HPUX_PARISC_ACC
static inline char *ltoa(int n) 
{
   static char buf [21];
   sprintf(buf, "%d", n);
   return(buf);
}
#endif 

// Set the trace levels
// These levels will be compared against a trace level mask to determine
// if a specific trace level is enabled 

const Uint32 Tracer::LEVEL1 = (1 << 0);
const Uint32 Tracer::LEVEL2 = (1 << 1);
const Uint32 Tracer::LEVEL3 = (1 << 2);
const Uint32 Tracer::LEVEL4 = (1 << 3);

// Set the Enter and Exit messages
const char Tracer::_FUNC_ENTER_MSG[] = "Entering method";
const char Tracer::_FUNC_EXIT_MSG[]  = "Exiting method";

// Set Log messages
const char Tracer::_LOG_MSG1[] = "LEVEL1 not enabled with Tracer::trace call.";
const char Tracer::_LOG_MSG2[]="Use trace macros, PEG_FUNC_ENTER/PEG_FUNC_EXIT";

// Initialize singleton instance of Tracer
Tracer* Tracer::_tracerInstance = 0;
 
// Set component separator
const char Tracer::_COMPONENT_SEPARATOR = ',';

// Set the number of defined components
const Uint32 Tracer::_NUM_COMPONENTS = 
    sizeof(TRACE_COMPONENT_LIST)/sizeof(TRACE_COMPONENT_LIST[0]);

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
    for (int index=0;index < _NUM_COMPONENTS; 
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
           _LOG_MSG1,_LOG_MSG2); 
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
           _LOG_MSG1,_LOG_MSG2); 
     }
     else
     {
         if (_isTraceEnabled(traceComponent,traceLevel))
         {
	    message = new char[strlen(fileName)+strlen(ltoa((long)lineNum))+6];
            sprintf(message,"[%s:%d]: ",fileName,lineNum);
            _trace(traceComponent,message,fmt,argList); 
	    delete []message;
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
        message = new char[strlen(fileName)+strlen(ltoa((long)lineNum))+6];
        sprintf(message,"[%s:%d]: ",fileName,lineNum);
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
        message = new char[strlen(fileName)+strlen(ltoa((long)lineNum))+6];
        sprintf(message,"[%s:%d]: ",fileName,lineNum);
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
    if ((traceComponent < 0) || (traceComponent >= _NUM_COMPONENTS ))
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
    Uint32 retCode;

    // Get the current system time and prepend to message
    String currentTime = System::getCurrentASCIITime();
    ArrayDestroyer<char> timeStamp(currentTime.allocateCString());

    // Allocate messageHeader. 
    msgHeader = new char [strlen(message)
        + strlen(TRACE_COMPONENT_LIST[traceComponent]) 
	+ strlen(timeStamp.getPointer()) + 6];

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
        sprintf(msgHeader,"%s: %s ",timeStamp.getPointer(),
            TRACE_COMPONENT_LIST[traceComponent] );
    }

    // Call trace file handler to write message
    _getInstance()->_traceHandler->handleMessage(msgHeader,fmt,argList);
    delete []msgHeader;
}

////////////////////////////////////////////////////////////////////////////////
//Set the trace file
////////////////////////////////////////////////////////////////////////////////
Uint32 Tracer::setTraceFile(const char* traceFile)
{
    ofstream outFile;
    Uint32 retCode = 0;

    // Check if the file can be opened in append mode
    if (traceFile)
    {
        outFile.open(traceFile,ofstream::app);

        if (outFile.good())
        {
            _getInstance()->_traceHandler->setFileName (traceFile);
            outFile.close();
        }
        else
        { 
	    outFile.close();
	    retCode = 1;
        }
    }
    else 
    {
        retCode=1;
    }
    return retCode;
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
//Set components to be traced. 
//The String traceComponents will be updated within the function.
////////////////////////////////////////////////////////////////////////////////
void Tracer::setTraceComponents(String traceComponents)
{
    Uint32 position=0;
    Uint32 index=0;
    String componentName;

    if (traceComponents != String::EMPTY)
    {
	// Check if ALL is specified
	if (traceComponents == "ALL")
	{
	    for (int index=0; index < _NUM_COMPONENTS;
		_getInstance()->_traceComponentMask[index++] = true);
            return; 
        }

        // initialise ComponentMask array to False
        for (int index = 0;index < _NUM_COMPONENTS; 
	    _getInstance()->_traceComponentMask[index++] = false);

 	// Append _COMPONENT_SEPARATOR to the end of the traceComponents
        traceComponents += _COMPONENT_SEPARATOR;

        while (traceComponents != String::EMPTY)
        {
            // Get the Component name from traceComponents. 
	    // Components are separated by _COMPONENT_SEPARATOR
            position = traceComponents.find(_COMPONENT_SEPARATOR);
 	    componentName = traceComponents.subString(0,(position));

	    // Lookup the index for Component name in TRACE_COMPONENT_LIST
            index = 0;
	    while (index < _NUM_COMPONENTS)
	    {
	        if (componentName == TRACE_COMPONENT_LIST[index])
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
            traceComponents.remove(0,position+1);
        }
    }
    else
    {
        // initialise ComponentMask array to False
        for (int index = 0;index < _NUM_COMPONENTS; 
	    _getInstance()->_traceComponentMask[index++] = false);
    }
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

PEGASUS_NAMESPACE_END

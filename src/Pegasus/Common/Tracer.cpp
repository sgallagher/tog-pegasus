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
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#2498
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
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

// Set the Enter and Exit messages
const char Tracer::_METHOD_ENTER_MSG[] = "Entering method";
const char Tracer::_METHOD_EXIT_MSG[]  = "Exiting method";

// Set Log messages
const char Tracer::_LOG_MSG[] = "LEVEL1 may only be used with trace macros PEG_METHOD_ENTER/PEG_METHOD_EXIT.";

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
    _traceHandler.reset(new TraceFileHandler());
    _traceLevelMask=0;
    _traceComponentMask.reset(new Boolean[_NUM_COMPONENTS]);

    // Initialize ComponentMask array to false
    for (Uint32 index=0;index < _NUM_COMPONENTS;
	(_traceComponentMask.get())[index++]=false);
}

////////////////////////////////////////////////////////////////////////////////
//Tracer destructor
////////////////////////////////////////////////////////////////////////////////
Tracer::~Tracer()
{
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
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
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
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
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
		_STRLEN_MAX_UNSIGNED_INT + (_STRLEN_MAX_PID_TID * 2) + 8 ];
            sprintf(
               message,
#if defined(PEGASUS_OS_VMS)
               //
               // pegasus_thread_self returns long-long-unsigned.
               //
               "[%d:%llu:%s:%u]: ",
//               "[%x:%llx:%s:%u]: ",
               System::getPID(),
               pegasus_thread_self(),
#else
               "[%d:%u:%s:%u]: ",
               System::getPID(),
               Uint32(pegasus_thread_self()),
#endif
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
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
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
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
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
//Traces the given string
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceString(
    const Uint32   traceComponent,
    const Uint32   traceLevel,
    const String&  traceString)
{
    if ( traceLevel == LEVEL1 )
    {
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
    }
    else
    {
        if (_isTraceEnabled(traceComponent,traceLevel))
        {
            trace(traceComponent,traceLevel,"%s",
                       (const char *)traceString.getCString());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//Traces the given string - Overloaded to include the fileName and line number
//of trace origin.
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceString(
    const char*   fileName,
    const Uint32  lineNum,
    const Uint32  traceComponent,
    const Uint32  traceLevel,
    const String& traceString)
{
    if ( traceLevel == LEVEL1 )
    {
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
    }
    else
    {
        if ( _isTraceEnabled( traceComponent, traceLevel ) )
        {
            trace(fileName,lineNum,traceComponent,traceLevel,"%s",
                     (const char *)traceString.getCString());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//Traces the message in the given CIMException object.
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceCIMException(
    const Uint32 traceComponent,
    const Uint32 traceLevel,
    CIMException cimException)
{
    if ( traceLevel == LEVEL1 )
    {
        trace( traceComponent, Tracer::LEVEL4, "%s", _LOG_MSG );
    }
    else
    {
        if ( _isTraceEnabled( traceComponent, traceLevel ) )
        {
            // get the CIMException trace message string
            String traceMsg =
                TraceableCIMException(cimException).getTraceDescription();

            // trace the string
            _traceString(traceComponent, traceLevel, traceMsg);
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
			    _STRLEN_MAX_UNSIGNED_INT + (_STRLEN_MAX_PID_TID * 2) + 8 ];
	
#if defined(PEGASUS_OS_VMS)
        //
        // pegasus_thread_self returns long-long-unsigned.
        //
        sprintf(
           message,
           "[%d:%llu:%s:%u]: ",
           System::getPID(),
           pegasus_thread_self(),
           fileName,
           lineNum);
#else
        sprintf(
           message,
           "[%d:%u:%s:%u]: ",
           System::getPID(),
           Uint32(pegasus_thread_self()),
           fileName,
           lineNum);
#endif
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
			    _STRLEN_MAX_UNSIGNED_INT + (_STRLEN_MAX_PID_TID * 2) + 8 ];
	
#if defined(PEGASUS_OS_VMS)
        //
        // pegasus_thread_self returns long-long-unsigned.
        //
        sprintf(
           message,
           "[%d:%llu:%s:%u]: ",
           System::getPID(),
           pegasus_thread_self(),
           fileName,
           lineNum);
#else
        sprintf(
           message,
           "[%d:%u:%s:%u]: ",
           System::getPID(),
           Uint32(pegasus_thread_self()),
           fileName,
           lineNum);
#endif
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
    return (((instance->_traceComponentMask.get())[traceComponent]) &&
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
    CString timeStamp = currentTime.getCString();

    //
    // Allocate messageHeader.
    // Needs to be updated if additional info is added
    //



    // Construct the message header
    // The message header is in the following format
    // timestamp: <component name> [file name:line number]
    if (*message != '\0')
    {
       // << Wed Jul 16 10:58:40 2003 mdd >> _STRLEN_MAX_PID_TID is not used in this format string
       msgHeader = new char [strlen(message)
			     + strlen(TRACE_COMPONENT_LIST[traceComponent])
			     + strlen(timeStamp) + _STRLEN_MAX_PID_TID + 5];

        sprintf(msgHeader,"%s: %s %s",(const char*)timeStamp,
            TRACE_COMPONENT_LIST[traceComponent] ,message);
        //delete [] msgHeader;
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
	tmpBuffer = new char[_STRLEN_MAX_PID_TID + 6];
#if defined(PEGASUS_OS_VMS)
        //
        // pegasus_thread_self returns long-long-unsigned.
        //
        sprintf(tmpBuffer, "[%u:%llu]: ", System::getPID(),
                pegasus_thread_self());
#else
        sprintf(tmpBuffer, "[%u:%u]: ", System::getPID(),
                Uint32(pegasus_thread_self()));
#endif
	msgHeader = new char [ strlen(timeStamp) + strlen(TRACE_COMPONENT_LIST[traceComponent]) +
			       strlen(tmpBuffer) + 1  + 5 ];
	
        sprintf(msgHeader,"%s: %s %s ",(const char*)timeStamp,
            TRACE_COMPONENT_LIST[traceComponent] ,tmpBuffer );
        delete []tmpBuffer;
        //delete [] msgHeader;
	
    }

    // Call trace file handler to write message
    _getInstance()->_traceHandler->handleMessage(msgHeader,fmt,argList);

    delete [] msgHeader;
}

////////////////////////////////////////////////////////////////////////////////
//Validate the trace file
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::isValidFileName(const char* filePath)
{
    String moduleName = _getInstance()->_moduleName;
    if (moduleName == String::EMPTY)
    {
        return (_getInstance()->_traceHandler->isValidFilePath(filePath));
    }
    else
    {
        String extendedFilePath = String(filePath) + "." + moduleName;
        return (_getInstance()->_traceHandler->isValidFilePath(
            extendedFilePath.getCString()));
    }
}

////////////////////////////////////////////////////////////////////////////////
//Validate the trace components
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::isValidComponents(const String& traceComponents)
{
    String invalidComponents;
    return isValidComponents(traceComponents, invalidComponents);
}

Boolean Tracer::isValidComponents(
    const String& traceComponents, String& invalidComponents)
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
        componentStr.append(_COMPONENT_SEPARATOR);

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
		invalidComponents.append(componentName);
		invalidComponents.append(_COMPONENT_SEPARATOR);
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
//Set the name of the module being traced
////////////////////////////////////////////////////////////////////////////////
void Tracer::setModuleName(const String& moduleName)
{
    _getInstance()->_moduleName = moduleName;
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
    if (*traceFile == 0)
    {
        return 1;
    }

    String moduleName = _getInstance()->_moduleName;
    if (moduleName == String::EMPTY)
    {
        return (_getInstance()->_traceHandler->setFileName(traceFile));
    }
    else
    {
        String extendedTraceFile = String(traceFile) + "." + moduleName;
        return (_getInstance()->_traceHandler->setFileName(
            extendedTraceFile.getCString()));
    }
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
void Tracer::setTraceComponents(const String& traceComponents)
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
                    (_getInstance()->_traceComponentMask.get())[index++] = true);
            return ;
        }

        // initialise ComponentMask array to False
        for (index = 0;index < _NUM_COMPONENTS;
	        (_getInstance()->_traceComponentMask.get())[index++] = false);

 	// Append _COMPONENT_SEPARATOR to the end of the traceComponents
        componentStr.append(_COMPONENT_SEPARATOR);

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
                    (_getInstance()->_traceComponentMask.get())[index]=true;

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
                 (_getInstance()->_traceComponentMask.get())[index++] = false);
    }
    return ;
}

#endif

PEGASUS_NAMESPACE_END

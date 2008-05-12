//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// Set the trace levels
// These levels will be compared against a trace level mask to determine
// if a specific trace level is enabled

const Uint32 Tracer::LEVEL0 =  0;
const Uint32 Tracer::LEVEL1 = (1 << 0);
const Uint32 Tracer::LEVEL2 = (1 << 1);
const Uint32 Tracer::LEVEL3 = (1 << 2);
const Uint32 Tracer::LEVEL4 = (1 << 3);
const Uint32 Tracer::LEVEL5 = (1 << 4);

// Set the Enter and Exit messages
const char Tracer::_METHOD_ENTER_MSG[] = "Entering method";
const char Tracer::_METHOD_EXIT_MSG[]  = "Exiting method";

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
const Uint32 Tracer::_STRLEN_MAX_PID_TID = 21;

// Initialize public indicator of trace state
Boolean Tracer::_traceOn = false;

////////////////////////////////////////////////////////////////////////////////
// Tracer constructor
// Constructor is private to preclude construction of Tracer objects
// Single Instance of Tracer is maintained for each process.
////////////////////////////////////////////////////////////////////////////////
Tracer::Tracer()
    : _traceComponentMask(new Boolean[_NUM_COMPONENTS]),
      _traceLevelMask(0),
      _traceHandler(new TraceFileHandler())
{
    // Initialize ComponentMask array to false
    for (Uint32 index=0;index < _NUM_COMPONENTS;
        (_traceComponentMask.get())[index++]=false);
    // NO componets are set
    _componentsAreSet=false;
}

////////////////////////////////////////////////////////////////////////////////
//Tracer destructor
////////////////////////////////////////////////////////////////////////////////
Tracer::~Tracer()
{
    delete _tracerInstance;
}


////////////////////////////////////////////////////////////////////////////////
//Traces the given message - Overloaded for including FileName and Line number
////////////////////////////////////////////////////////////////////////////////
void Tracer::_trace(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const char* fmt,
    va_list argList)
{
    char* message;
    //
    // Allocate memory for the message string
    // Needs to be updated if additional info is added
    //
    message = new char[strlen(fileName) +
        _STRLEN_MAX_UNSIGNED_INT + (_STRLEN_MAX_PID_TID * 2) + 8];
    sprintf(
       message,
       "[%d:%s:%s:%u]: ",
       System::getPID(),
       Threads::id().buffer,
       fileName,
       lineNum);

    _trace(traceComponent, message, fmt, argList);
    delete [] message;
}

////////////////////////////////////////////////////////////////////////////////
//Traces the message in the given CIMException object.
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceCIMException(
    const Uint32 traceComponent,
    const CIMException& cimException)
{        
    // get the CIMException trace message string
    CString traceMsg =
        TraceableCIMException(cimException).getTraceDescription().getCString();
    // trace the string
    _traceCString(traceComponent, "", (const char*) traceMsg);
}

SharedArrayPtr<char> Tracer::getHTTPRequestMessage(
    const Buffer& requestMessage)
{
    const Uint32 requestSize = requestMessage.size();

    // Make a copy of the request message.
    SharedArrayPtr<char>
        requestBuf(new char [requestSize + 1]);
    strncpy(requestBuf.get(), requestMessage.getData(), requestSize);
    requestBuf.get()[requestSize] = 0;

    //
    // Check if requestBuffer contains a Basic authorization header.
    // If true, suppress the user/passwd info in the request buffer.
    //
    char* sep;
    const char* line = requestBuf.get();

    while ((sep = HTTPMessage::findSeparator(
        line, (Uint32)(requestSize - (line - requestBuf.get())))) &&
        (line != sep))
    {
        if (HTTPMessage::expectHeaderToken(line, "Authorization") &&
             HTTPMessage::expectHeaderToken(line, ":") &&
             HTTPMessage::expectHeaderToken(line, "Basic"))
        {
            // Suppress the user/passwd info
            HTTPMessage::skipHeaderWhitespace(line);
            for ( char* userpass = (char*)line ; 
                userpass < sep; 
                *userpass = 'X', userpass++);

            break;
        }

        line = sep + ((*sep == '\r') ? 2 : 1);
    }

    return requestBuf;
}

////////////////////////////////////////////////////////////////////////////////
//Traces method entry and exit
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceMethod(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const char* methodEntryExit,
    const char* method)
{
    char* message;

    //
    // Allocate memory for the message string
    // Needs to be updated if additional info is added
    //
    // assume Method entry/exit string 15 characters long
    // +1 space character
    message = new char[ strlen(fileName) +
        _STRLEN_MAX_UNSIGNED_INT + (_STRLEN_MAX_PID_TID * 2) + 8 
        + 16];

    sprintf(
       message,
       "[%d:%s:%s:%u]: %s ",
       System::getPID(),
       Threads::id().buffer,
       fileName,
       lineNum,
       methodEntryExit);
        
    _traceCString(traceComponent, message, method);

    delete [] message;
}


////////////////////////////////////////////////////////////////////////////////
//Checks if trace is enabled for the given component and level
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::isTraceEnabled(
    const Uint32 traceComponent,
    const Uint32 traceLevel)
{
    Tracer* instance = _getInstance();
    if (traceComponent >= _NUM_COMPONENTS)
    {
        return false;
    }
    return (((instance->_traceComponentMask.get())[traceComponent]) &&
            (traceLevel & instance->_traceLevelMask));
}

////////////////////////////////////////////////////////////////////////////////
//Called by all trace interfaces with variable arguments
//to log message to trace file
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
       // << Wed Jul 16 10:58:40 2003 mdd >> _STRLEN_MAX_PID_TID is not used
       // in this format string
       msgHeader = new char [strlen(message) +
           strlen(TRACE_COMPONENT_LIST[traceComponent]) +
           strlen(timeStamp) + _STRLEN_MAX_PID_TID + 5];

        sprintf(msgHeader, "%s: %s %s", (const char*)timeStamp,
            TRACE_COMPONENT_LIST[traceComponent], message);
    }
    else
    {
        //
        // Since the message is blank, form a string using the pid and tid
        //
        char* tmpBuffer;

        //
        // Allocate messageHeader.
        // Needs to be updated if additional info is added
        //
        tmpBuffer = new char[2 * _STRLEN_MAX_PID_TID + 6];
        sprintf(tmpBuffer, "[%u:%s]: ",
            System::getPID(), Threads::id().buffer);
        msgHeader = new char[strlen(timeStamp) +
            strlen(TRACE_COMPONENT_LIST[traceComponent]) +
            strlen(tmpBuffer) + 1  + 5];

        sprintf(msgHeader, "%s: %s %s ", (const char*)timeStamp,
            TRACE_COMPONENT_LIST[traceComponent], tmpBuffer);
        delete [] tmpBuffer;
    }

    // Call trace file handler to write message
    _getInstance()->_traceHandler->handleMessage(msgHeader,fmt,argList);

    delete [] msgHeader;
}

////////////////////////////////////////////////////////////////////////////////
//Called by all trace interfaces using a character string without format string
//to log message to trace file
////////////////////////////////////////////////////////////////////////////////
void Tracer::_traceCString(
    const Uint32 traceComponent,
    const char* message,
    const char* cstring)
{
    char* completeMessage;

    // Get the current system time and prepend to message
    String currentTime = System::getCurrentASCIITime();
    CString timeStamp = currentTime.getCString();
    //
    // Allocate completeMessage.
    // Needs to be updated if additional info is added
    //

    // Construct the message header
    // The message header is in the following format
    // timestamp: <component name> [file name:line number]
    if (*message != '\0')
    {
       // << Wed Jul 16 10:58:40 2003 mdd >> _STRLEN_MAX_PID_TID is not used
       // in this format string
       completeMessage = new char [strlen(message) +
           strlen(TRACE_COMPONENT_LIST[traceComponent]) +
           strlen(timeStamp) + _STRLEN_MAX_PID_TID + 5 +
           strlen(cstring) ];

        sprintf(completeMessage, "%s: %s %s%s", (const char*)timeStamp,
            TRACE_COMPONENT_LIST[traceComponent], message, cstring);
    }
    else
    {
        //
        // Since the message is blank, form a string using the pid and tid
        //
        char* tmpBuffer;

        //
        // Allocate messageHeader.
        // Needs to be updated if additional info is added
        //
        tmpBuffer = new char[2 * _STRLEN_MAX_PID_TID + 6];
        sprintf(tmpBuffer, "[%u:%s]: ",
            System::getPID(), Threads::id().buffer);

        completeMessage = new char[strlen(timeStamp) +
            strlen(TRACE_COMPONENT_LIST[traceComponent]) +
            strlen(tmpBuffer) + 1  + 5 +
            strlen(cstring)];

        sprintf(completeMessage, "%s: %s %s %s", (const char*)timeStamp,
            TRACE_COMPONENT_LIST[traceComponent], tmpBuffer, cstring);
        delete [] tmpBuffer;
    }

    // Call trace file handler to write message
    _getInstance()->_traceHandler->handleMessage(completeMessage);

    delete [] completeMessage;
}


////////////////////////////////////////////////////////////////////////////////
//Validate the trace file
////////////////////////////////////////////////////////////////////////////////
Boolean Tracer::isValidFileName(const char* filePath)
{
    String moduleName = _getInstance()->_moduleName;
    if (moduleName == String::EMPTY)
    {
        return _getInstance()->_traceHandler->isValidFilePath(filePath);
    }
    else
    {
        String extendedFilePath = String(filePath) + "." + moduleName;
        return _getInstance()->_traceHandler->isValidFilePath(
            extendedFilePath.getCString());
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
    const String& traceComponents,
    String& invalidComponents)
{
    // Validate the trace components and modify the traceComponents argument
    // to reflect the invalid components

    Uint32    position=0;
    Uint32    index=0;
    String    componentName;
    String    componentStr;
    Boolean   validComponent=false;
    Boolean   retCode=true;

    componentStr = traceComponents;
    invalidComponents = String::EMPTY;

    if (componentStr != String::EMPTY)
    {
        // Check if ALL is specified
        if (String::equalNoCase(componentStr,"ALL"))
        {
            return true;
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

            if (!validComponent)
            {
                invalidComponents.append(componentName);
                invalidComponents.append(_COMPONENT_SEPARATOR);
            }
        }
    }
    else
    {
        // trace components is empty, it is a valid value so return true
        return true;
    }

    if (invalidComponents != String::EMPTY)
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
// interfaces. This section defines the trace functions IF the remove
// trace flag is NOT set.  If it is set, they are defined as empty functions
// in the header file.

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
        return _getInstance()->_traceHandler->setFileName(traceFile);
    }
    else
    {
        String extendedTraceFile = String(traceFile) + "." + moduleName;
        return _getInstance()->_traceHandler->setFileName(
            extendedTraceFile.getCString());
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
        case LEVEL0:
            _getInstance()->_traceLevelMask = 0x00;
            break;

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

        case LEVEL5:
            _getInstance()->_traceLevelMask = 0x1F;
            break;

        default:
            _getInstance()->_traceLevelMask = 0x00;
            retCode = 1;
    }

    if (_getInstance()->_componentsAreSet && 
        _getInstance()->_traceLevelMask )
    {
        _traceOn = true;
    } 
    else
    {
        _traceOn = false;
    }

    return retCode;
}

////////////////////////////////////////////////////////////////////////////////
// Set components to be traced.
////////////////////////////////////////////////////////////////////////////////
void Tracer::setTraceComponents(const String& traceComponents)
{
    Tracer* instance = _getInstance();

    // Check if ALL is specified
    if (String::equalNoCase(traceComponents,"ALL"))
    {
        for (Uint32 index = 0; index < _NUM_COMPONENTS; index++)
        {
            (instance->_traceComponentMask.get())[index] = true;
        }

        instance->_componentsAreSet=true;

        // If tracing isn't turned off by a traceLevel of zero, let's
        // turn on the flag that activates tracing.
        _traceOn = (instance->_traceLevelMask != LEVEL0);

        return;
    }

    // initialize ComponentMask array to False
    for (Uint32 index = 0; index < _NUM_COMPONENTS; index++)
    {
        (instance->_traceComponentMask.get())[index] = false;
    }
    _traceOn = false;
    instance->_componentsAreSet=false;

    if (traceComponents != String::EMPTY)
    {
        Uint32 index = 0;
        Uint32 position = 0;
        String componentName;
        String componentStr = traceComponents;

   
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
                    (instance->_traceComponentMask.get())[index] = true;

                    instance->_componentsAreSet=true;

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

        // If one of the components was set for tracing and the traceLevel
        // is not zero, then turn on tracing.
        _traceOn = (instance->_componentsAreSet &&
                   (instance->_traceLevelMask != LEVEL0));
    }

    return ;
}

void Tracer::traceEnter(
    TracerToken& token,
    const char* file,
    size_t line,
    Uint32 traceComponent,
    const char* method)
{
    token.component = traceComponent;
    token.method = method;
    
    if (isTraceEnabled(traceComponent, LEVEL5))
    {
        _traceMethod(
            file, (Uint32)line, traceComponent, 
            _METHOD_ENTER_MSG, method);
    }
}

void Tracer::traceExit(
    TracerToken& token,
    const char* file,
    size_t line)
{
    if (isTraceEnabled(token.component, LEVEL5) && token.method)
        _traceMethod(
            file, (Uint32)line, token.component,
            _METHOD_EXIT_MSG, token.method);
}

////////////////////////////////////////////////////////////////////////////////
//Traces the given string - Overloaded to include the fileName and line number
//of trace origin.
////////////////////////////////////////////////////////////////////////////////
void Tracer::traceCString(
    const char* fileName,
    const Uint32 lineNum,
    const Uint32 traceComponent,
    const char* cstring)
{
    char* message;

    //
    // Allocate memory for the message string
    // Needs to be updated if additional info is added
    //
    message = new char[strlen(fileName) +
        _STRLEN_MAX_UNSIGNED_INT + (_STRLEN_MAX_PID_TID * 2) + 8];
    sprintf(
       message,
       "[%d:%s:%s:%u]: ",
       System::getPID(),
       Threads::id().buffer,
       fileName,
       lineNum);

    _traceCString(traceComponent, message, cstring);
    delete [] message;
}

void Tracer::traceCIMException(
    const Uint32 traceComponent,
    const Uint32 traceLevel,
    const CIMException& cimException)
{
    if (isTraceEnabled(traceComponent, traceLevel))
    {
        _traceCIMException(traceComponent, cimException);
    }
}

#endif /* !PEGASUS_REMOVE_TRACE */

PEGASUS_NAMESPACE_END

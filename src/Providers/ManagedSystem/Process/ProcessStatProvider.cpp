//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//=============================================================================
//
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Jim Metcalfe
//              Carlos Bonilla
//              Mike Glantz         <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#define CAPTION "UnixProcessStatisticalInformation"
#define DESCRIPTION \
 "The class PG_UnixProcessStatisticalInformation contains statistical " \
 "information about a process."

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "ProcessStatProvider.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define NAMESPACE_SYSTEM "root/cimv2"

ProcessStatProvider::ProcessStatProvider()
{
  // cout << "ProcessStatProvider::ProcessStatProvider()" << endl;
}

ProcessStatProvider::~ProcessStatProvider()
{
  // cout << "ProcessStatProvider::~ProcessStatProvider()" << endl;
}

/*
================================================================================
NAME              : createInstance
DESCRIPTION       : Create a PG_UnixProcessStatisticalInformation instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::createInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
                    ResponseHandler<CIMReference> &handler)

{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/*
================================================================================
NAME              : deleteInstance
DESCRIPTION       : Delete a PG_UnixProcessStatisticalInformation instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::deleteInstance(const OperationContext       &context,
                    const CIMReference           &instanceReference,
                    ResponseHandler<CIMInstance> &handler)

{ 
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/*
================================================================================
NAME              : enumerateInstances
DESCRIPTION       : Enumerates all of the PG_UnixProcessStatisticalInformation instances.
                    An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::enumerateInstances(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler)
{   
  // cout << "ProcessStatProvider::enumerateInstances()" << endl;

    _chkInit(context);

    Process _p;
    int                 pIndex;
    String              className;

    /* Validate the classname.  */
    className = classReference.getClassName();
    _checkClass(className);

    /* Notify processing is starting. */
    handler.processing();

    /* Get the process information and construct an instance for each */
    /* process.                                                       */
    for (pIndex = 0; _p.getProcessInfo(pIndex); )
    {
        /* Deliver the instance. */
        handler.deliver(_constructInstance(CLASS_PG_UNIX_PROCESS_STAT,_p));
    }

    /* Notify processing is complete. */
    handler.complete();

    return;
}  /* enumerateInstances */


/*
================================================================================
NAME              : enumerateInstanceNames
DESCRIPTION       : Enumerates all of the PG_UnixProcessStatisticalInformation instance names.
                    An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::enumerateInstanceNames(const OperationContext &ctx,
                            const CIMReference &ref,
                            ResponseHandler<CIMReference> &handler)
{
    // cout << "ProcessStatProvider::enumerateInstanceNames()" << endl;

    _chkInit(ctx);

    int                 pIndex;
    Process _p;
    String              className;
    
    /* Validate the classname.  */
    className = ref.getClassName();
    _checkClass(className);

    String ns = ref.getNameSpace();

    /* Notify processing is starting. */
    handler.processing();

    /* Get the process information and construct the key bindings for */
    /* each process.                                                  */
    // Note that getProcessInfo modifies pIndex to point to the
    // next process structure
    for (pIndex = 0; _p.getProcessInfo(pIndex); )
    {
            Array<KeyBinding> keyBindings;

            /* Construct the key bindings. */
            keyBindings.append(KeyBinding(PROPERTY_CS_CREATION_CLASS_NAME,
	    	                          CLASS_CIM_UNITARY_COMPUTER_SYSTEM,
                                          KeyBinding::STRING));
		
            keyBindings.append(KeyBinding(PROPERTY_CS_NAME,
                                          _getCSName(),
                                          KeyBinding::STRING));
    
            keyBindings.append(KeyBinding(PROPERTY_OS_CREATION_CLASS_NAME,
		                          CLASS_CIM_OPERATING_SYSTEM,
                                          KeyBinding::STRING));
		
            keyBindings.append(KeyBinding(PROPERTY_OS_NAME,
                                          _getOSName(),
                                          KeyBinding::STRING));

            keyBindings.append(KeyBinding(PROPERTY_PROCESS_CREATION_CLASS_NAME,
		                          CLASS_PG_UNIX_PROCESS_STAT,
                                          KeyBinding::STRING));

            keyBindings.append(KeyBinding(PROPERTY_HANDLE,
                                          _p.getHandle(),
                                          KeyBinding::STRING));

            keyBindings.append(KeyBinding(PROPERTY_NAME,
                                          _p.getHandle(),
                                          KeyBinding::STRING));

            /* Deliver the names. */
            handler.deliver(CIMReference(_getCSName(), // hostname
                                         ref.getNameSpace(),
                                         CLASS_PG_UNIX_PROCESS_STAT,
                                         keyBindings));

    } /* for */

    /* Notify processing is complete. */
    handler.complete();

    return;

}  /* enumerateInstanceNames */

/*
================================================================================
NAME              : getInstance
DESCRIPTION       : Returns a single instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::getInstance(const OperationContext &ctx,
                 const CIMReference           &instanceName,
                 const Uint32                  flags,
                 const Array<String>          &propertyList,
                 ResponseHandler<CIMInstance> &handler)
{	
    // cout << "ProcessStatProvider::getInstance()" << endl;

    _chkInit(ctx);

    KeyBinding         kb;
    String             className;
    String             keyName;
    String             handle;
    int                pid;
    int                i;
    int                numberKeys = 0;
    Process _p;
    int                pIndex;

    /* Validate the classname.  */
    className = instanceName.getClassName();
    _checkClass(className);

    String ns = instanceName.getNameSpace();

    /* Extract the key values. */
    Array<KeyBinding> kbArray = instanceName.getKeyBindings();

    /* Validate the key bindings.  There are six keys for this class. */
    for(i=0;i<kbArray.size();i++)
    {
        if ( kbArray.size() != 6)
	{
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}

	kb = kbArray[i];

	keyName = kb.getName();

	if (String::equalNoCase(keyName, PROPERTY_CS_CREATION_CLASS_NAME)
	    && String::equalNoCase(kb.getValue(),
                             CLASS_CIM_UNITARY_COMPUTER_SYSTEM))	   
	{
	    numberKeys++;
	}
	else if (String::equalNoCase(keyName, PROPERTY_CS_NAME)
                 && String::equalNoCase(kb.getValue(), _getCSName()))
	{
	    numberKeys++;
	}
	else if (String::equalNoCase(keyName,
                               PROPERTY_OS_CREATION_CLASS_NAME)
	         && String::equalNoCase(kb.getValue(),
                                  CLASS_CIM_OPERATING_SYSTEM))	   
	{
	    numberKeys++;
	}
	else if (String::equalNoCase(keyName, PROPERTY_OS_NAME)
	         && String::equalNoCase(kb.getValue(), _getOSName()))	   
	{
	    numberKeys++;
	}
	else if (String::equalNoCase(keyName, PROPERTY_PROCESS_CREATION_CLASS_NAME)
	         && String::equalNoCase(kb.getValue(), CLASS_PG_UNIX_PROCESS_STAT))	   
	{
	    numberKeys++;
	}
	else if (String::equal(keyName, PROPERTY_HANDLE))
	{
            handle = kb.getValue();
	    numberKeys++;
	}
	// There must be a Name key and it must have the same value
	// as the Handle
	else if (String::equal(keyName, PROPERTY_NAME)
	         && String::equal(kb.getValue(),handle))
	{
	    // name = kb.getValue();
	    numberKeys++;
	}
	else
	{
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}
		
    } /* for */

    if(numberKeys != 7)
    {
        throw CIMException(CIM_ERR_NOT_FOUND);
    }
	    
    /* Find the instance.  First convert the instance id which is the */
    /* process handle to an integer.  This is necessary because the   */
    /* handle is the process id on HP-UX which must be passed to      */
    /* pstat_getproc() as an integer.                                 */

    char *h = handle.allocateCString();
    pid = atoi(h);
    delete [] h;

    /* Get the process information. */
    // step through processes; if this loop finishes, there's a problem
    for (pIndex = 0; _p.getProcessInfo(pIndex); )
    {
        char *h = _p.getHandle().allocateCString();
        int pid2 = atoi(h);
        delete [] h;
        if (pid == pid2)
        {
            /* Notify processing is starting. */
            handler.processing();

            /* Return the instance. */
            handler.deliver(_constructInstance(CLASS_PG_UNIX_PROCESS_STAT,_p));

            /* Notify processing is complete. */
            handler.complete();
            return;
        }
    }
    throw CIMException(CIM_ERR_NOT_FOUND);

    return;
}

/*
================================================================================
NAME              : modifyInstance
DESCRIPTION       : Modify a PG_UnixProcessStatisticalInformation instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::modifyInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
		    const Uint32                 flags,
		    const Array<String>          &propertyList,
                    ResponseHandler<CIMInstance> &handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/*
================================================================================
NAME              : initialize
DESCRIPTION       : Initializes the provider.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::initialize(CIMOMHandle &ch)
{
  // cout << "ProcessStatProvider::initialize()" << endl;
  _ch = ch;
  
  // Normally we would obtain CSName and OSName during
  // initialization from instances of PG_ComputerSystem and
  // PG_OperatingSystem, respectively, but since CIM operations
  // through CIMOMHandle require a context parameter, we will
  // do initialization, if necessary, in the other methods which
  // are passed this parameter. Meanwhile, set flag.
  _initialized = false;

  return;  
}  /* initialize */

/*
================================================================================
NAME              : terminate
DESCRIPTION       : Terminates the provider.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessStatProvider::terminate()
{
  // cout << "ProcessStatProvider::terminate()" << endl;
}

/*
================================================================================
NAME              : _constructInstance
DESCRIPTION       : Constructs the instance's property values.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        : instance            (IN/OUT)
================================================================================
*/
CIMInstance ProcessStatProvider::_constructInstance(const String &className,
                                         const Process &_p) 
{
  String s;
  Uint32 i32;
  Uint64 i64;
  CIMDateTime d;

  CIMInstance inst(className);

// CIM_ManagedElement

//   string Caption
  inst.addProperty(CIMProperty(PROPERTY_CAPTION,CAPTION));

//   string Description
  inst.addProperty(CIMProperty(PROPERTY_DESCRIPTION,DESCRIPTION));

// PG_UnixProcessStatisticalInformation

//   [ key ] string CSCreationClassName
  inst.addProperty(CIMProperty(PROPERTY_CS_CREATION_CLASS_NAME,
                               CLASS_CIM_UNITARY_COMPUTER_SYSTEM));

//   [ key ] string CSName
  inst.addProperty(CIMProperty(PROPERTY_CS_NAME,_getCSName()));

//   [ key ] string OSCreationClassName
  inst.addProperty(CIMProperty(PROPERTY_OS_CREATION_CLASS_NAME,
                               CLASS_CIM_OPERATING_SYSTEM));

//   [ key ] string OSName
  inst.addProperty(CIMProperty(PROPERTY_OS_NAME,_getOSName()));

//   [ key ] string ProcessCreationClassName
  inst.addProperty(CIMProperty(PROPERTY_PROCESS_CREATION_CLASS_NAME,className));

//   [ key ] string Handle
  inst.addProperty(CIMProperty(PROPERTY_HANDLE,_p.getHandle()));

//   [ key ] string Name ATTN: *** use Handle ***
  inst.addProperty(CIMProperty(PROPERTY_NAME,_p.getHandle()));

// PG_UnixProcessStatisticalInformation

//    uint32 CPUTime
  if (_p.getCPUTime(i32))
    inst.addProperty(CIMProperty(PROPERTY_CPU_TIME,i32));

//    uint64 RealText
  if (_p.getRealText(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_TEXT,i64));

//    uint64 RealData
  if (_p.getRealData(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_DATA,i64));

//    uint64 RealStack
  if (_p.getRealStack(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_STACK,i64));

//    uint64 VirtualText
  if (_p.getVirtualText(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_TEXT,i64));

//    uint64 VirtualData
  if (_p.getVirtualData(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_DATA,i64));

//    uint64 VirtualStack
  if (_p.getVirtualStack(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_STACK,i64));

//    uint64 VirtualMemoryMappedFileSize
  if (_p.getVirtualMemoryMappedFileSize(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_MEMORY_MAPPED_FILESIZE,i64));

//    uint64 VirtualSharedMemory
  if (_p.getVirtualSharedMemory(i64))
    inst.addProperty(CIMProperty(PROPERTY_VIRTUAL_SHARED_MEMORY,i64));

//    uint64 CpuTimeDeadChildren
  if (_p.getCpuTimeDeadChildren(i64))
    inst.addProperty(CIMProperty(PROPERTY_CPU_TIME_DEAD_CHILDREN,i64));

//    uint64 SystemTimeDeadChildren
  if (_p.getSystemTimeDeadChildren(i64))
    inst.addProperty(CIMProperty(PROPERTY_SYSTEM_TIME_DEAD_CHILDREN,i64));

//    uint64 RealSpace
  if (_p.getRealSpace(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_SPACE,i64));
 
  return inst;
}


/*
================================================================================
NAME              : _getOSName
DESCRIPTION       : Call uname() and get the operating system name.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             : 
================================================================================
*/
String ProcessStatProvider::_getOSName()
{
  return _osName;
}


/*
================================================================================
NAME              : _getCSName
DESCRIPTION       : return the hostname.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             :
================================================================================
*/
String ProcessStatProvider::_getCSName()
{
  return _hostName;
}

void ProcessStatProvider::_checkClass(String& className)
{
    if (!String::equalNoCase(className, CLASS_PG_UNIX_PROCESS_STAT))
    {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
}

/*
================================================================================
NAME              : _chkInit
DESCRIPTION       : initializes private members if not already done
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   : 
NOTES             :
================================================================================
*/
void ProcessStatProvider::_chkInit(const OperationContext &c)
{
  if (_initialized) return;

  Process _p;

  // try to obtain instance of CIM_UnitaryComputerSystem
  Array<CIMReference> csNames = _ch.enumerateInstanceNames(
                                  c,
                                  NAMESPACE_SYSTEM,
                                  CLASS_CIM_UNITARY_COMPUTER_SYSTEM);
  if (csNames.size() != 0)
  // we got an array of instanceNames; use only the first
  {
    Array<KeyBinding> kb = csNames[0].getKeyBindings();
    for (int i=0; i<kb.size(); i++)
    {
      if (kb[i].getName() == "Name")
      {
        _hostName = kb[i].getValue();
        break;
      }
    }
  }
  else
  // That didn't work; call platform-specific code
  {
    _hostName = _p.getCSName();
  }                                 

  // try to obtain instance of CIM_OperatingSystem
  Array<CIMReference> osNames = _ch.enumerateInstanceNames(
                                  c,
                                  NAMESPACE_SYSTEM,
                                  "PG_OperatingSystem");
  if (osNames.size() != 0)
  // Use first instance of array
  {
    Array<KeyBinding> kb = osNames[0].getKeyBindings();
    for (int i=0; i<kb.size(); i++)
    {
      if (kb[i].getName() == "Name")
      {
        _osName = kb[i].getValue();
        return;
      }
    }
  }
  else
  // Fall back to uname()
  {
    _osName = _p.getOSName();
  }
  return;
}


PEGASUS_NAMESPACE_END

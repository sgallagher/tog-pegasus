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

#define CAPTION "UnixProcess"
#define DESCRIPTION "The class PG_UnixProcess represents a running process"

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "ProcessProvider.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define NAMESPACE_SYSTEM "root/cimv2"

ProcessProvider::ProcessProvider()
{
  // cout << "ProcessProvider::ProcessProvider()" << endl;
}

ProcessProvider::~ProcessProvider()
{
  // cout << "ProcessProvider::~ProcessProvider()" << endl;
}

/*
================================================================================
NAME              : createInstance
DESCRIPTION       : Create a PG_UnixProcess instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessProvider::createInstance(const OperationContext &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
                    ResponseHandler<CIMReference> &handler)

{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/*
================================================================================
NAME              : deleteInstance
DESCRIPTION       : Delete a PG_UnixProcess instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessProvider::deleteInstance(const OperationContext &context,
                    const CIMReference           &instanceReference,
                    ResponseHandler<CIMInstance> &handler)

{ 
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

/*
================================================================================
NAME              : enumerateInstances
DESCRIPTION       : Enumerates all of the PG_UnixProcess instances.
                    An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessProvider::enumerateInstances(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	ResponseHandler<CIMInstance> & handler)
{   
    // cout << "ProcessProvider::enumerateInstances()" << endl;

    _chkInit(context);

    int                 pIndex;
    String              className;
    Process _p;

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
        handler.deliver(_constructInstance(CLASS_PG_UNIX_PROCESS, _p));
    }

    /* Notify processing is complete. */
    handler.complete();

    return;
}  /* enumerateInstances */


/*
================================================================================
NAME              : enumerateInstanceNames
DESCRIPTION       : Enumerates all of the PG_UnixProcess instance names.
                    An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : 
PARAMETERS        :
================================================================================
*/
void ProcessProvider::enumerateInstanceNames(const OperationContext &ctx,
                            const CIMReference &ref,
                            ResponseHandler<CIMReference> &handler)
{
    // cout << "ProcessProvider::enumerateInstanceNames()" << endl;

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

            keyBindings.append(KeyBinding(PROPERTY_CREATION_CLASS_NAME,
		                          CLASS_PG_UNIX_PROCESS,
                                          KeyBinding::STRING));

            keyBindings.append(KeyBinding(PROPERTY_HANDLE,
                                          _p.getHandle(),
                                          KeyBinding::STRING));

            /* Deliver the names. */
            handler.deliver(CIMReference(_getCSName(), // hostname
                                         ref.getNameSpace(),
                                         CLASS_PG_UNIX_PROCESS,
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
void ProcessProvider::getInstance(const OperationContext &ctx,
                 const CIMReference           &instanceName,
                 const Uint32                  flags,
                 const CIMPropertyList        &propertyList,
                 ResponseHandler<CIMInstance> &handler)
{	
    // cout << "ProcessProvider::getInstance()" << endl;

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

	if (String::equal(keyName, PROPERTY_CS_CREATION_CLASS_NAME)
	    && String::equal(kb.getValue(),
                             CLASS_CIM_UNITARY_COMPUTER_SYSTEM))	   
	{
	    numberKeys++;
	}
	else if (String::equal(keyName, PROPERTY_CS_NAME)
                 && String::equal(kb.getValue(), _getCSName()))
	{
	    numberKeys++;
	}
	else if (String::equal(keyName,
                               PROPERTY_OS_CREATION_CLASS_NAME)
	         && String::equal(kb.getValue(),
                                  CLASS_CIM_OPERATING_SYSTEM))	   
	{
	    numberKeys++;
	}
	else if (String::equal(keyName, PROPERTY_OS_NAME)
	         && String::equal(kb.getValue(), _getOSName()))	   
	{
	    numberKeys++;
	}
	else if (String::equal(keyName, PROPERTY_CREATION_CLASS_NAME)
	         && String::equal(kb.getValue(), CLASS_PG_UNIX_PROCESS))	   
	{
	    numberKeys++;
	}
	else if (String::equal(keyName, PROPERTY_HANDLE))
	{
            handle = kb.getValue();
	    numberKeys++;
	}
	else
	{
	    throw CIMException(CIM_ERR_INVALID_PARAMETER);
	}
		
    } /* for */

    if(numberKeys != 6)
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
            handler.deliver(_constructInstance(CLASS_PG_UNIX_PROCESS,_p));

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
DESCRIPTION       : Modify a PG_UnixProcess instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void ProcessProvider::modifyInstance(const OperationContext &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
		    const Uint32                 flags,
		    const CIMPropertyList        &propertyList,
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
void ProcessProvider::initialize(CIMOMHandle &ch)
{
  // cout << "ProcessProvider::initialize()" << endl;
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
void ProcessProvider::terminate()
{
  // cout << "ProcessProvider::terminate()" << endl;
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
CIMInstance ProcessProvider::_constructInstance(const String &className,
                                                const Process &_p) 
{
  String s;
  Array<String> as;
  Uint16 i16;
  Uint32 i32;
  Uint64 i64;
  CIMDateTime d;

  CIMInstance inst(className);

// CIM_ManagedElement

//   string Caption
  inst.addProperty(CIMProperty(PROPERTY_CAPTION,CAPTION));

//   string Description
  inst.addProperty(CIMProperty(PROPERTY_DESCRIPTION,DESCRIPTION));

// CIM_ManagedSystemElement

//   datetime InstallDate
  if (_p.getInstallDate(d))
    inst.addProperty(CIMProperty(PROPERTY_INSTALL_DATE,d));

//   string Name    // Populated in CIM_Process

//   string Status
  if (_p.getStatus(s))
    inst.addProperty(CIMProperty(PROPERTY_STATUS,s));

// CIM_LogicalElement
//   ** No local properties **

// CIM_Process

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

//   string Name
  if (_p.getName(s))
    inst.addProperty(CIMProperty(PROPERTY_NAME,s));

//   [ key ] string CreationClassName
  inst.addProperty(CIMProperty(PROPERTY_CREATION_CLASS_NAME,className));

//   [ key ] string Handle
  inst.addProperty(CIMProperty(PROPERTY_HANDLE,_p.getHandle()));

//   uint32 Priority
  if (_p.getPriority(i32))
    inst.addProperty(CIMProperty(PROPERTY_PRIORITY,i32));

//   uint16 ExecutionState
  if (_p.getExecutionState(i16))
    inst.addProperty(CIMProperty(PROPERTY_EXECUTION_STATE,i16));

//   string OtherExecutionDescription
  if (_p.getOtherExecutionDescription(s))
    inst.addProperty(CIMProperty(PROPERTY_OTHER_EXECUTION_DESCRIPTION,s));

//   datetime CreationDate
  if (_p.getCreationDate(d))
    inst.addProperty(CIMProperty(PROPERTY_CREATION_DATE,d));

//   datetime TerminationDate
  if (_p.getTerminationDate(d))
    inst.addProperty(CIMProperty(PROPERTY_TERMINATION_DATE,d));

//   uint64 KernelModeTime
  if (_p.getKernelModeTime(i64))
    inst.addProperty(CIMProperty(PROPERTY_KERNEL_MODE_TIME,i64));

//   uint64 UserModeTime
  if (_p.getUserModeTime(i64))
    inst.addProperty(CIMProperty(PROPERTY_USER_MODE_TIME,i64));

//   uint64 WorkingSetSize
  if (_p.getWorkingSetSize(i64))
    inst.addProperty(CIMProperty(PROPERTY_WORKING_SET_SIZE,i64));

// PG_UnixProcess

//    string ParentProcessID
  if (_p.getParentProcessID(s))
    inst.addProperty(CIMProperty(PROPERTY_PARENT_PROCESS_ID,s));

//    uint64 RealUserID
  if (_p.getRealUserID(i64))
    inst.addProperty(CIMProperty(PROPERTY_REAL_USER_ID,i64));

//    uint64 ProcessGroupID
  if (_p.getProcessGroupID(i64))
    inst.addProperty(CIMProperty(PROPERTY_PROCESS_GROUP_ID,i64));

//    uint64 ProcessSessionID
  if (_p.getProcessSessionID(i64))
    inst.addProperty(CIMProperty(PROPERTY_PROCESS_SESSION_ID,i64));

//    string ProcessTTY
  if (_p.getProcessTTY(s))
    inst.addProperty(CIMProperty(PROPERTY_PROCESS_TTY,s));

//    string ModulePath
  if (_p.getModulePath(s))
    inst.addProperty(CIMProperty(PROPERTY_MODULE_PATH,s));

//    string Parameters[]
  if (_p.getParameters(as))
    inst.addProperty(CIMProperty(PROPERTY_PARAMETERS,as));

//    uint32 ProcessNiceValue
  if (_p.getProcessNiceValue(i32))
    inst.addProperty(CIMProperty(PROPERTY_PROCESS_NICE_VALUE,i32));

//    string ProcessWaitingForEvent
  if (_p.getProcessWaitingForEvent(s))
    inst.addProperty(CIMProperty(PROPERTY_PROCESS_WAITING_FOR_EVENT,s));

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
String ProcessProvider::_getOSName()
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
String ProcessProvider::_getCSName()
{
  return _hostName;
}

void ProcessProvider::_checkClass(String& className)
{
    if (!String::equalNoCase(className, CLASS_CIM_PROCESS) &&
        !String::equalNoCase(className, CLASS_PG_UNIX_PROCESS))
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
void ProcessProvider::_chkInit(const OperationContext &c)
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

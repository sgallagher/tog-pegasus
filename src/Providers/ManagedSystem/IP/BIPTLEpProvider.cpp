//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By:
//         David Kennedy       <dkennedy@linuxcare.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//         Al Stone, Hewlett-Packard Company <ahs3@fc.hp.com>
//         Jim Metcalfe, Hewlett-Packard Company
//         Carlos Bonilla, Hewlett-Packard Company
//         Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//         Lyle Wilkinson, Hewlett-Packard Company <lyle_wilkinson@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////


// ==========================================================================
// Includes.
// ==========================================================================

#include "BIPTLEpProvider.h"

// ==========================================================================
// Class names:
//
// We use CIM_UnitaryComputerSystem as the value of the key
// SystemCreationClassName, because this class has properties that
// are important for clients of this provider.
// ==========================================================================

#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM  "CIM_UnitaryComputerSystem"
#define CLASS_CIM_IP_PROTOCOL_ENDPOINT     "CIM_IPProtocolEndpoint"
#define CLASS_CIM_LAN_ENDPOINT             "CIM_LANEndpoint"
#define CLASS_PG_BINDS_IP_TO_LAN_ENDPOINT  "PG_BindsIPToLANEndpoint"

// ==========================================================================
// The number of keys for the classes.
// ==========================================================================

#define NUMKEYS_PG_BINDS_IP_TO_LAN_ENDPOINT   2
#define NUMKEYS_CIM_PROTOCOL_ENDPOINT         4

// ==========================================================================
// Property names.  These values are returned by the provider as
// the property names.
// ==========================================================================

// Properties in PG_BindsIPToLANEndpoint

// References
//      CIM_LANEndpoint REF Antecedent
#define PROPERTY_ANTECEDENT          "Antecedent"
//      CIM_IPProtocolEndpoint REF Dependent
#define PROPERTY_DEPENDENT           "Dependent"

// Regular Properties
#define PROPERTY_FRAME_TYPE                  "FrameType"

// Properties that make up the References

#define PROPERTY_NAME                        "Name"
#define PROPERTY_CREATION_CLASS_NAME         "CreationClassName"
#define PROPERTY_SYSTEM_CREATION_CLASS_NAME  "SystemCreationClassName"
#define PROPERTY_SYSTEM_NAME                 "SystemName"


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


BIPTLEpProvider::BIPTLEpProvider()
{
  cout << "BIPTLEpProvider::BIPTLEpProvider()" << endl;
}

BIPTLEpProvider::~BIPTLEpProvider()
{
  
}


/*
================================================================================
NAME              : createInstance
DESCRIPTION       : Create a IPProtocolEndpoint instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void BIPTLEpProvider::createInstance(const OperationContext &context,
                    const CIMObjectPath           &instanceName,
                    const CIMInstance            &instanceObject,
                    ResponseHandler<CIMObjectPath> &handler)

{
  throw NotSupported(String::EMPTY);
}

/*
================================================================================
NAME              : deleteInstance
DESCRIPTION       : Delete a IPProtocolEndpoint instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void BIPTLEpProvider::deleteInstance(const OperationContext &context,
                    const CIMObjectPath           &instanceReference,
                    ResponseHandler<void> &handler)

{
  throw NotSupported(String::EMPTY);
}

/*
================================================================================
NAME              : enumerateInstances
DESCRIPTION       : Enumerates all of the IPProtocolEndpoint instances.
                  : An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : LocalOnly, DeepInheritance and propertyList are not
                  : respected by this provider. Localization is not supported
PARAMETERS        :
================================================================================
*/
void BIPTLEpProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    cout << "BIPTLEpProvider::enumerateInstances()" << endl;

    String className = classReference.getClassName();
    String nameSpace = classReference.getNameSpace();

    // Validate the classname
    _checkClass(className);

    // Notify processing is starting
    handler.processing();

    int i;
    InterfaceList _ifList;

    for (i = 0; i < _ifList.size(); i++)
    {
	IPInterface _ipif = _ifList.getInterface(i);

	if (_ipif.bindsToLANInterface())
	{
	    handler.deliver(
		_constructInstance(CLASS_PG_BINDS_IP_TO_LAN_ENDPOINT,
		     nameSpace, _ipif ) );
        }
    }

    // Notify processing is complete
    handler.complete();

    return;
}  // enumerateInstances

/*
================================================================================
NAME              : enumerateInstanceNames
DESCRIPTION       : Enumerates all of the IPProtocolEndpoint instance names.
                  : An array of instance references is returned.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Localization is not supported by this provider.
PARAMETERS        :
================================================================================
*/
void BIPTLEpProvider::enumerateInstanceNames(const OperationContext &ctx,
                            const CIMObjectPath &ref,
                            ResponseHandler<CIMObjectPath> &handler)
{
    cout << "BIPTLEpProvider::enumerateInstanceNames()" << endl;

    String className = ref.getClassName();
    String nameSpace = ref.getNameSpace();

    // Validate the classname
    _checkClass(className);

    // Notify processing is starting
    handler.processing();

    int i;
    InterfaceList _ifList;

    cout << "BIPTLEpProvider::enumerateInstanceNames() _ifList Initialized"
   	 << endl;

    for (i = 0; i < _ifList.size(); i++)
    {
	IPInterface _ipif = _ifList.getInterface(i);

	if (_ipif.bindsToLANInterface())
	{
	    // Deliver the names
	    handler.deliver(
		CIMObjectPath(String::EMPTY, // hostname
			nameSpace, CLASS_PG_BINDS_IP_TO_LAN_ENDPOINT,
			_constructKeyBindings(_ipif) ) );
	}
    }

    // Notify processing is complete
    handler.complete();

    return;

}  // enumerateInstanceNames

/*
================================================================================
NAME              : getInstance
DESCRIPTION       : Returns a single instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : LocalOnly, DeepInheritance and propertyList are not
                  : respected by this provider. Localization is not supported
PARAMETERS        :
================================================================================
*/
void BIPTLEpProvider::getInstance(const OperationContext &ctx,
                 const CIMObjectPath           &instanceName,
                 const Uint32                  flags,
                 const CIMPropertyList        &propertyList,
                 ResponseHandler<CIMInstance> &handler)
{	
  cout << "BIPTLEpProvider::getInstance(" << instanceName << ")" << endl;

  KeyBinding kb;
  String className = instanceName.getClassName();
  String nameSpace = instanceName.getNameSpace();
  int i;
  int keysFound;  // this will be used as a bit array
  String sn;      // system name
  String lepName, // LAN Endpoint Name
	 ipeName, // IP Protocol Endpoint Name
	 refCCN,  // Reference's Creation Class Name
	 refName; // Reference's Name

  // Grab the system name
  if (IPInterface::getSystemName(sn) == false)
     sn = String::EMPTY;

  // Validate the classname
  _checkClass(className);

  // Extract the key values
  Array<KeyBinding> kbArray = instanceName.getKeyBindings();

  // Leave immediately if wrong number of keys
  if ( kbArray.size() != NUMKEYS_PG_BINDS_IP_TO_LAN_ENDPOINT )
    throw InvalidParameter("Wrong number of keys");

  // Validate the keys.
  // Each loop iteration will set a bit in keysFound when a valid
  // key is found. If the expected bits aren't all set when
  // the loop finishes, it's a problem
  for (i=0, keysFound=0; i < NUMKEYS_PG_BINDS_IP_TO_LAN_ENDPOINT; i++)
  {
    kb = kbArray[i];

    String keyName = kb.getName();
    String keyValue = kb.getValue();

    // Antecedent must match
    if (String::equalNoCase(keyName, PROPERTY_ANTECEDENT))
    {
      if (_goodPERefKeys(keyValue, refCCN, refName) &&
	  String::equalNoCase(refCCN, CLASS_CIM_LAN_ENDPOINT) )
      {
        keysFound |= 1;
	lepName = refName;
        cout << "BIPTLEpProvider::getInstance(): lepName=" << lepName << endl;
      }
      else
         throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }

    // Dependent must match
    else if (String::equalNoCase(keyName, PROPERTY_DEPENDENT))
    {
      if (_goodPERefKeys(keyValue, refCCN, refName) &&
	  String::equalNoCase(refCCN, CLASS_CIM_IP_PROTOCOL_ENDPOINT) )
      {
        keysFound |= 2;
	ipeName = refName;
        cout << "BIPTLEpProvider::getInstance(): ipeName=" << ipeName << endl;
      }
      else
         throw InvalidParameter(keyValue+": bad value for key "+keyName);
    }

    // Key name was not recognized by any of the above tests
    else throw InvalidParameter(keyName+ ": Unrecognized key");
		
  } // for

  // We could get here if we didn't get all the keys, which
  // could happen if the right number of keys were supplied,
  // and they all had valid names and values, but there were
  // any duplicates (e.g., two Names, no SystemName)
  if (keysFound != (1<<NUMKEYS_PG_BINDS_IP_TO_LAN_ENDPOINT)-1)
    throw InvalidParameter("Bad object name");

  // Get the Interface List
  InterfaceList _ifList;
  IPInterface _ipif;

  // Make sure the LAN Endpoint name is embedded in the IP Protocol Endpoint
  // name.  If we can find the request interface and if it's one that
  // binds to a LAN Interface, then we've found the right one, so
  // return it to the client.
  if (ipeName.find(lepName) != PEG_NOT_FOUND &&
      _ifList.findInterface(ipeName, _ipif) &&
      _ipif.bindsToLANInterface())
  {
    /* Notify processing is starting. */
    handler.processing();

    /* Return the instance. */
    handler.deliver(_constructInstance(className, nameSpace, _ipif));

    /* Notify processing is complete. */
    handler.complete();
    return;
  }

  throw ObjectNotFound(ipeName+": No such IP Interface");

  return; // can never execute, but required to keep compiler happy
}

/*
================================================================================
NAME              : modifyInstance
DESCRIPTION       : Modify a IPProtocolEndpoint instance.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : Currently not supported.
PARAMETERS        :
================================================================================
*/
void BIPTLEpProvider::modifyInstance(const OperationContext &context,
                    const CIMObjectPath           &instanceName,
                    const CIMInstance            &instanceObject,
		    const Uint32                 flags,
		    const CIMPropertyList        &propertyList,
                    ResponseHandler<void> &handler)
{
  // Could be supported in the future for certain properties
  throw NotSupported(String::EMPTY);
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
void BIPTLEpProvider::initialize(CIMOMHandle &ch)
{
  cout << "BIPTLEpProvider::initialize()" << endl;

  _cimomHandle = ch;

  // call platform-specific routine to initialize System Name
  // This hides whether or not the platform-specific code wants
  // to set up a static System Name or dynamically obtain it
  // each time.
  IPInterface::initSystemName();

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
void BIPTLEpProvider::terminate()
{
  cout << "BIPTLEpProvider::terminate()" << endl;
  delete this;
}

/*
================================================================================
NAME              : _constructKeyBindings
DESCRIPTION       : Constructs an array of keybindings for an IP Interface
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
PARAMETERS        : className, Process
================================================================================
*/
Array<KeyBinding> BIPTLEpProvider::_constructKeyBindings(
					const IPInterface& _ipif)
{
  cout << "BIPTLEpProvider::_constructKeyBindings()" << endl;

  Array<KeyBinding> keyBindings;
  String s;
  String _a, _d;   // Antecedent and Dependent REF strings

  // Reference BNF:  <Qualifyingclass>.<key1>=<value1>[,<keyx>=<valuex>]*

  // Build the references

  _a = String(CLASS_CIM_LAN_ENDPOINT) + String(".") +
       String(PROPERTY_SYSTEM_CREATION_CLASS_NAME) + String("=\"") +
       String(CLASS_CIM_UNITARY_COMPUTER_SYSTEM) + String("\",") +
       String(PROPERTY_SYSTEM_NAME) + String("=\"");

  _d = String(CLASS_CIM_IP_PROTOCOL_ENDPOINT) + String(".") +
       String(PROPERTY_SYSTEM_CREATION_CLASS_NAME) + String("=\"") +
       String(CLASS_CIM_UNITARY_COMPUTER_SYSTEM) + String("\",") +
       String(PROPERTY_SYSTEM_NAME) + String("=\"");

  if (_ipif.getSystemName(s))
  {
      _a += s;
      _d += s;
  }
  else
  {
	throw NotSupported(
		String("Host-specific module doesn't support Key `") +
		String(PROPERTY_SYSTEM_NAME) + String("'"));
  }

  _a += String("\",") + String(PROPERTY_CREATION_CLASS_NAME) + String("=\"") +
	String(CLASS_CIM_LAN_ENDPOINT) + String("\",") +
	String(PROPERTY_NAME) + String("=\"") + _ipif.get_LANInterfaceName();

  _d += String("\",") + String(PROPERTY_CREATION_CLASS_NAME) + String("=\"") +
	String(CLASS_CIM_IP_PROTOCOL_ENDPOINT) + String("\",") +
	String(PROPERTY_NAME) + String("=\"");

  if (_ipif.getName(s))
  {
	_d += s;
  }
  else
  {
	throw NotSupported(
		String("Host-specific module doesn't support Key `") +
		String(PROPERTY_NAME) + String("'"));
  }

  _a += "\"";
  _d += "\"";

  cout << "BIPTLEpProvider::_constructKeyBindings(): Antecedent = `" +
	_a + "'"  << endl;
  cout << "BIPTLEpProvider::_constructKeyBindings(): Dependent = `" +
	_d + "'"  << endl;

  // Construct the key bindings
  keyBindings.append(KeyBinding(PROPERTY_ANTECEDENT,
	    	                _a, KeyBinding::REFERENCE));

  keyBindings.append(KeyBinding(PROPERTY_DEPENDENT,
		                _d, KeyBinding::REFERENCE));

  cout << "BIPTLEpProvider::_constructKeyBindings() -- done" << endl;

  return keyBindings;
}

/*
================================================================================
NAME              : _constructInstance
DESCRIPTION       : Constructs instance by adding its properties. The
                  : IP Interface argument has already been filled in
                  : with data from an existing IP Interface
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
PARAMETERS        : className, Process
================================================================================
*/
CIMInstance BIPTLEpProvider::_constructInstance(const String &className,
                                                const String &nameSpace,
                                                const IPInterface &_ipif)
{
  cout << "BIPTLEpProvider::_constructInstance()" << endl;

  String s;
  Uint16 i16;
  CIMDateTime d;

  CIMInstance inst(className);

  // Set path

  inst.setPath(CIMObjectPath(String::EMPTY, // hostname
                             nameSpace,
                             CLASS_PG_BINDS_IP_TO_LAN_ENDPOINT,
                             _constructKeyBindings(_ipif)));

// ======================================================
// The following properties are in CIM_ServiceAccessPoint
// ======================================================

  // The keys for this class are:
  // CIM_LANEndpoint REF Antecedent
  // CIM_IPProtocolEndpoint REF Dependent

  // Rather than rebuilding the key properties, we will reuse
  // the values that were inserted for us in the ObjectPath,
  // trusting that this was done correctly

  // Get the keys
  Array<KeyBinding> key = inst.getPath().getKeyBindings();
  // loop through keys, inserting them as properties
  // luckily, all keys for this class are strings, so no
  // need to check key type
  for (int i=0; i<key.size(); i++)
  {
    // add a property created from the name and value
    inst.addProperty(CIMProperty(key[i].getName(),key[i].getValue()));
  }

// CIM_BindsToLANEndpoint

//   uint16 FrameType
  if (_ipif.getFrameType(i16))
    inst.addProperty(CIMProperty(PROPERTY_FRAME_TYPE,i16));

  cout << "BIPTLEpProvider::_constructInstance() -- done" << endl;

  return inst;
}

/*
================================================================================
NAME              : _goodPERefKeys
DESCRIPTION       : Checks to see if this is a value reference to a
		  : Protocol Endpoint with good values for SCCN, SN,
		  : and CCN.  If it is, it returns the Name;
                  : otherwise, it returns false.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean BIPTLEpProvider::_goodPERefKeys(const CIMObjectPath &instName,
				     String &rccn,    // CreationClassName
				     String &rname)   // Name
{
   cout << "BIPTLEpProvider::_goodPERefKeys(" << instName << ")" << endl;

   int keysFound,  // this will be used as a bit array
       i; 

   Array<KeyBinding> kbArray = instName.getKeyBindings();

   if ( kbArray.size() != NUMKEYS_CIM_PROTOCOL_ENDPOINT)
     throw InvalidParameter("Wrong number of keys in reference");
    
   for (i=0, keysFound=0; i < NUMKEYS_CIM_PROTOCOL_ENDPOINT; i++)
   {
      KeyBinding kb = kbArray[i];  

      String keyName = kb.getName();
      String keyValue = kb.getValue();

      cout << "BIPTLEpProvider::_goodPERefKeys(): keyName=" << keyName <<
              ", keyValue=" << keyValue << endl;

      // SystemCreationClassName
      if (String::equalNoCase(keyName, PROPERTY_SYSTEM_CREATION_CLASS_NAME))
      {
	  if (String::equalNoCase(keyValue,CLASS_CIM_UNITARY_COMPUTER_SYSTEM) ||
		  String::equal(keyValue, String::EMPTY) )
             keysFound |= 1;
	  else
	     return false;  // invalid value for SystemCreationClassName
      }

      // SystemName
      else if (String::equalNoCase(keyName, PROPERTY_SYSTEM_NAME))
      {
	  String sn; // System Name
	  if (IPInterface::getSystemName(sn) == false)
	      sn = String::EMPTY;

	  if (String::equalNoCase(keyValue,sn) ||
		  String::equal(keyValue, String::EMPTY) )
             keysFound |= 2;
	  else
	     return false;  // invalid value for SystemName
      }

      // CreationClassName
      else if (String::equalNoCase(keyName, PROPERTY_CREATION_CLASS_NAME))
      {
	    rccn = keyValue;
            keysFound |= 4;
      }

      // Name must be a valid IP interface, but we will know that later
      // For now, just verify that it's present
      else if (String::equalNoCase(keyName, PROPERTY_NAME))
      {
            rname = keyValue;
            keysFound |= 8;
      }

      // Key name was not recognized by any of the above tests
      else return false;

   } // for
   
   cout << "BIPTLEpProvider::_goodPERefKeys(): rccn =" << rccn <<
           ", rname=" << rname << endl;

  // We could get here if we didn't get all the keys, which
  // could happen if the right number of keys were supplied,
  // and they all had valid names and values, but there were
  // any duplicates (e.g., two Names, no SystemName)
   if (keysFound != (1<<NUMKEYS_CIM_PROTOCOL_ENDPOINT)-1)
      return false;

   cout << "BIPTLEpProvider::_goodPERefKeys() - success!" << endl;

   return true;
}

/*
================================================================================
NAME              : _checkClass
DESCRIPTION       : tests the argument for valid classname,
                  : throws exception if not
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
void BIPTLEpProvider::_checkClass(String& className)
{
  if (!String::equalNoCase(className, CLASS_PG_BINDS_IP_TO_LAN_ENDPOINT))
    throw NotSupported(className+": Class not supported");
}

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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//  Namespace Provider
//
//	This provider answers to the "false" class __namespace.  This is the
//	deprecated version of manipulation in the DMTF WBEM model.  This function
//	is defined in the CIM Operations over HTTP docuement.  However, while
//	the function exists, no class was ever defined in the CIM model for
//	__nemaspace.  Therefore each implementation is free to provide its own
//	class definition.
///////////////////////////////////////////////////////////////////////////////

/* STATUS: In process but running 30 April 2002 */

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <cctype>
#include <iostream>

#include "NamespaceProvider.h"
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Provider/OperationFlag.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
 * Specification for CIM Operations over HTTP
 *
 * Version 1.0
 *
 * 2.5. Namespace Manipulation
 * There are no intrinsic methods defined specifically for the
 * purpose of manipulating CIM Namespaces. However, the modelling
 * of a CIM Namespace using the class __Namespace, together with
 * the requirement that the root Namespace MUST be supported by
 * all CIM Servers, implies that all Namespace operations can be
 * supported.
 *
 * For example:
 *
 * Enumeration of all child Namespaces of a particular Namespace
 * is realized by calling the intrinsic method
 * EnumerateInstanceNames against the parent Namespace,
 * specifying a value for the ClassName parameter of __Namespace.
 *
 * Creation of a child Namespace is realized by calling the
 * intrinsic method CreateInstance against the parent Namespace,
 * specifying a value for the NewInstance parameter which defines
 * a valid instance of the class __Namespace and whose Name
 * property is the desired name of the new Namespace.
 *
*/

/**
    The constant representing the __namespace class name
*/
static const char NAMESPACE_CLASSNAME [] = "__Namespace";
static const char NAMESPACE_PROPERTYNAME [] = "Name";
static const char ROOTNS [] = "root";

Boolean _isNamespace(
        Array<String>& namespaceNames,
	String& namespaceName)

{
     Boolean found = false;
     for(Uint32 i = 0; i < namespaceNames.size(); i++)
     {
        if(String::equalNoCase(namespaceNames[i], namespaceName))
        {
            return true;
        }
     }
     return false;
}

Boolean _isChild(
        String& parentNamespaceName,
	String& namespaceName)

{
   //
   //  If length of namespace name is shorter than or equal to the
   //  length of parent namespace name, cannot be a child
   //
   if (namespaceName.size () <= parentNamespaceName.size ())
   {
      return false;
   }

   //
   //  Compare prefix substring of namespace name with parent namespace name
   //
   else if (String::equalNoCase (namespaceName.subString
             (0, parentNamespaceName.size ()), parentNamespaceName))
   {
      return true;
   }
   return false;
}

void _getKeyValue (
        const CIMInstance& namespaceInstance,
	String& childNamespaceName,
	Boolean& isRelativeName)

{
       //Validate key property

       Uint32 pos;
       CIMValue propertyValue;

       // [Key, MaxLen (256), Description (
       //       "A string that uniquely identifies the Namespace "
       //       "within the ObjectManager.") ]
       // string Name;

       pos = namespaceInstance.findProperty(NAMESPACE_PROPERTYNAME);
       if (pos == PEG_NOT_FOUND)
       {
           throw PropertyNotFound(NAMESPACE_PROPERTYNAME);
       }

       propertyValue = namespaceInstance.getProperty(pos).getValue();
       if (propertyValue.getType() != CIMTYPE_STRING)
       {
           throw InvalidParameter("Invalid type for property: "
                                 + String(NAMESPACE_PROPERTYNAME));
       }

       propertyValue.get(childNamespaceName);

       isRelativeName = !(childNamespaceName == String::EMPTY);

}

void _getKeyValue (
	const CIMObjectPath&  instanceName,
	String& childNamespaceName,
	Boolean& isRelativeName)

{

       Array<KeyBinding> kbArray = instanceName.getKeyBindings();
       if ((kbArray.size() == 1) &&
                (kbArray[0].getName() == NAMESPACE_PROPERTYNAME))
       {
           childNamespaceName = kbArray[0].getValue();
	   isRelativeName = !(childNamespaceName == String::EMPTY);
       }
       else
       {
           throw InvalidParameter("Invalid key property:  ");
       }
}

void _generateFullNamespaceName(
        Array<String>& namespaceNames,
	String& parentNamespaceName,
	String& childNamespaceName,
	Boolean isRelativeName,
	String& fullNamespaceName)

{
       // If isRelativeName is true, then the parentNamespace
       // MUST exist
       //
       if (isRelativeName)
       {
          if (!_isNamespace(namespaceNames, parentNamespaceName))
          {
             throw ObjectNotFound("Parent namespace does not exist: "
                                      + parentNamespaceName);
          }
          // Create full namespace name by prepending parentNamespaceName
          fullNamespaceName = parentNamespaceName + "/" + childNamespaceName;
        }
        else
        {
          fullNamespaceName = parentNamespaceName;
	}

}


void NamespaceProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
        const CIMInstance& myInstance,
	ResponseHandler<CIMObjectPath> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "NamespaceProvider::createInstance()");

	String childNamespaceName;
        String newNamespaceName;
	Boolean isRelativeName;

       // Verify that the className = __namespace
       if (!CIMName::equal(myInstance.getClassName(), NAMESPACE_CLASSNAME))
       {
	   PEG_METHOD_EXIT();
           throw NotSupported(myInstance.getClassName() +
			    " not supported by Namespace Provider");
       }

       //ATTN-DME-P3-20020522: ADD AUTHORIZATION CHECK TO __NAMESPACE PROVIDER
       String userName;
       try
       {
           IdentityContainer container = context.get(CONTEXT_IDENTITY);
           userName = container.getUserName();
       }
       catch (...)
       {
           userName = String::EMPTY;
       }

       _getKeyValue(myInstance, childNamespaceName, isRelativeName);
        String parentNamespaceName = instanceReference.getNameSpace();

	PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "childNamespaceName = " + childNamespaceName +
	       ", isRelativeName = " +
	       (isRelativeName?String("true"):String("false")) +
	       ", parentNamespaceName = " + parentNamespaceName);

	// begin processing the request
       handler.processing();

       _repository->write_lock();

       try
       {
           Array<String> namespaceNames;
	   namespaceNames = _repository->enumerateNameSpaces();

           _generateFullNamespaceName(namespaceNames, parentNamespaceName,
			             childNamespaceName, isRelativeName,
				     newNamespaceName);

	   _repository->createNameSpace(newNamespaceName);

	   PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "Namespace = " + newNamespaceName + " successfully created.");

       }
       catch(CIMException& e)
       {
	  _repository->write_unlock();
	  PEG_METHOD_EXIT();
	  throw e;
       }
       catch(Exception& e)
       {
	  _repository->write_unlock();
	  PEG_METHOD_EXIT();
	  throw e;
       }

       _repository->write_unlock();

       // return key (i.e., CIMObjectPath) for newly created namespace

       Array<KeyBinding> keyBindings;
       keyBindings.append(KeyBinding(NAMESPACE_PROPERTYNAME,
	         isRelativeName?childNamespaceName:parentNamespaceName,
                                     KeyBinding::STRING));
       CIMObjectPath newInstanceReference (String::EMPTY, parentNamespaceName,
                                     NAMESPACE_CLASSNAME, keyBindings);
       handler.deliver(newInstanceReference);

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return;
   }

void NamespaceProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceName,
	ResponseHandler<void> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "NamespaceProvider::deleteInstance");

	String childNamespaceName;
        String deleteNamespaceName;
	Boolean isRelativeName;

       // Verify that the className = __namespace
       if (!CIMName::equal(instanceName.getClassName(), NAMESPACE_CLASSNAME))
       {
	   PEG_METHOD_EXIT();
           throw NotSupported(instanceName.getClassName() +
			    " not supported by Namespace Provider");
       }

       //ATTN-DME-P3-20020522: ADD AUTHORIZATION CHECK TO __NAMESPACE PROVIDER
       String userName;
       try
       {
           IdentityContainer container = context.get(CONTEXT_IDENTITY);
           userName = container.getUserName();
       }
       catch (...)
       {
           userName = String::EMPTY;
       }

       _getKeyValue(instanceName, childNamespaceName, isRelativeName);
       String parentNamespaceName = instanceName.getNameSpace();

       PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "childNamespaceName = " + childNamespaceName +
	       (isRelativeName?String("true"):String("false")) +
	       ", parentNamespaceName = " + parentNamespaceName);

       // begin processing the request
       handler.processing();

       _repository->write_lock();

       try
       {
           Array<String> namespaceNames;
	   namespaceNames = _repository->enumerateNameSpaces();

           _generateFullNamespaceName(namespaceNames, parentNamespaceName,
			             childNamespaceName, isRelativeName,
				     deleteNamespaceName);

	   if (String::equalNoCase(deleteNamespaceName, ROOTNS))
           {
               throw NotSupported("root namespace may be deleted.");
           }

	   _repository->deleteNameSpace(deleteNamespaceName);

	   PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "Namespace = " + deleteNamespaceName + " successfully deleted.");

       }
       catch(CIMException& e)
       {
	  _repository->write_unlock();
	  PEG_METHOD_EXIT();
	  throw e;
       }
       catch(Exception& e)
       {
	  _repository->write_unlock();
          PEG_METHOD_EXIT();
	  throw e;
       }

       _repository->write_unlock();

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return ;
    }

void NamespaceProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceName,
        const Uint32 flags,
        const CIMPropertyList & properatyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "NamespaceProvider::getInstance");

	String childNamespaceName;
        String getNamespaceName;
	Boolean isRelativeName;

       // Verify that the className = __namespace
       if (!CIMName::equal(instanceName.getClassName(), NAMESPACE_CLASSNAME))
       {
	   PEG_METHOD_EXIT();
           throw NotSupported(instanceName.getClassName() +
			    " not supported by Namespace Provider");
       }

       //ATTN-DME-P3-20020522: ADD AUTHORIZATION CHECK TO __NAMESPACE PROVIDER
       String userName;
       try
       {
           IdentityContainer container = context.get(CONTEXT_IDENTITY);
           userName = container.getUserName();
       }
       catch (...)
       {
           userName = String::EMPTY;
       }

       _getKeyValue(instanceName, childNamespaceName, isRelativeName);
       String parentNamespaceName = instanceName.getNameSpace();

       PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "childNamespaceName = " + childNamespaceName +
	       (isRelativeName?String("true"):String("false")) +
	       ", parentNamespaceName = " + parentNamespaceName);

       // begin processing the request
       handler.processing();

       _repository->read_lock();

       try
       {
           Array<String> namespaceNames;
	   namespaceNames = _repository->enumerateNameSpaces();

           _generateFullNamespaceName(namespaceNames, parentNamespaceName,
			             childNamespaceName, isRelativeName,
				     getNamespaceName);

           if (!_isNamespace(namespaceNames, getNamespaceName))
           {
              throw ObjectNotFound("Namespace deos not exist: "
                                     + getNamespaceName);
           }

	   PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "Namespace = " + getNamespaceName + " successfully found.");

       }
       catch(CIMException& e)
       {
	  _repository->read_unlock();
	  PEG_METHOD_EXIT();
	  throw e;
       }
       catch(Exception& e)
       {
	  _repository->read_unlock();
          PEG_METHOD_EXIT();
	  throw e;
       }

       _repository->read_unlock();

       //Set name of class
       CIMInstance instance(NAMESPACE_CLASSNAME);

       //
       // construct the instance
       //
       instance.addProperty(CIMProperty(NAMESPACE_PROPERTYNAME,
	   isRelativeName?childNamespaceName:parentNamespaceName));
       //instance.setPath(instanceName);

       handler.deliver(instance);

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return ;
    }

void NamespaceProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & ref,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
       PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "NamespaceProvider::enumerateInstances()");

       // Verify that ClassName == __Namespace
       if (!CIMName::equal(ref.getClassName(), NAMESPACE_CLASSNAME))
       {
           PEG_METHOD_EXIT();
           throw NotSupported(ref.getClassName() +
			    " not supported by Namespace Provider");
       }

       //ATTN-DME-P3-20020522: ADD AUTHORIZATION CHECK TO __NAMESPACE PROVIDER
       String userName;
       try
       {
           IdentityContainer container = context.get(CONTEXT_IDENTITY);
           userName = container.getUserName();
       }
       catch (...)
       {
          userName = String::EMPTY;
       }

       String parentNamespaceName = ref.getNameSpace();

       PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "parentNamespaceName = " + parentNamespaceName);

       // begin processing the request
       handler.processing();

       Array<CIMInstance> instanceArray;

       _repository->read_lock();

       try
       {
	   Array<String> namespaceNames = _repository->enumerateNameSpaces();

	  // Build the instances. For now simply build the __Namespace instances
	  // Note that for the moment, the only property is name.
          for (Uint32 i = 0; i < namespaceNames.size(); i++)
          {
              if (_isChild(parentNamespaceName, namespaceNames[i]))
              {
                  CIMInstance instance(NAMESPACE_CLASSNAME);
                  instance.addProperty(
                     (CIMProperty(NAMESPACE_PROPERTYNAME,
		          namespaceNames[i].subString(parentNamespaceName.size()+1,
			   namespaceNames[i].size()-parentNamespaceName.size()-1))));
                  instanceArray.append(instance);
                  //instance.setPath(instanceName);
                  PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	                              "childNamespace = " + namespaceNames[i]);
              }
          }
       }
       catch(CIMException& e)
       {
	  _repository->read_unlock();
          PEG_METHOD_EXIT();
	  throw e;
       }
       catch(Exception& e)
       {
	  _repository->read_unlock();
          PEG_METHOD_EXIT();
	  throw e;
       }

       _repository->read_unlock();

       handler.deliver(instanceArray);

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
    }

void NamespaceProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
        ResponseHandler<CIMObjectPath> & handler)
    {

	PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "NamespaceProvider::enumerateInstanceNames()");


        // Verify that ClassName == __Namespace
        if (!CIMName::equal(classReference.getClassName(), NAMESPACE_CLASSNAME))
        {
	    PEG_METHOD_EXIT();
            throw NotSupported(classReference.getClassName() +
			    " not supported by Namespace Provider");
        }

        //ATTN-DME-P3-20020522: ADD AUTHORIZATION CHECK TO __NAMESPACE PROVIDER
	String userName;
        try
        {
            IdentityContainer container = context.get(CONTEXT_IDENTITY);
            userName = container.getUserName();
        }
        catch (...)
        {
           userName = String::EMPTY;
        }

       String parentNamespaceName = classReference.getNameSpace();
       PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "parentNamespaceName = " + parentNamespaceName);

       Array<CIMObjectPath> instanceRefs;

       _repository->read_lock();

       try
       {
	   Array<String> namespaceNames = _repository->enumerateNameSpaces();
	   Array<KeyBinding> keyBindings;

	  // Build the instances. For now simply build the __Namespace instances
	  // Note that for the moment, the only property is name.
          for (Uint32 i = 0; i < namespaceNames.size(); i++)
          {
              if (_isChild(parentNamespaceName, namespaceNames[i]))
              {
		  keyBindings.clear();
                  keyBindings.append(KeyBinding(NAMESPACE_PROPERTYNAME,
		          namespaceNames[i].subString(parentNamespaceName.size()+1,
			     namespaceNames[i].size()-parentNamespaceName.size()-1),
                             KeyBinding::STRING));
                  CIMObjectPath ref(String::EMPTY, parentNamespaceName,
				  NAMESPACE_CLASSNAME, keyBindings);
                  instanceRefs.append(ref);
                  PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	                                  "childNamespace = " + namespaceNames[i]);
              }
          }
       }
       catch(CIMException& e)
       {
	  _repository->read_unlock();
          PEG_METHOD_EXIT();
	  throw e;
       }
       catch(Exception& e)
       {
	  _repository->read_unlock();
          PEG_METHOD_EXIT();
	  throw e;
       }

       _repository->read_unlock();

	handler.deliver(instanceRefs);

	handler.complete();

        PEG_METHOD_EXIT();
    }

PEGASUS_NAMESPACE_END


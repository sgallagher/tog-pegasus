///%/////////////////////////////////////////////////////////////////////////////
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
//=============================================================================
//
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: 
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
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Provider/OperationFlag.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constants representing the string literals.
*/


static const char NAMESPACE_NAME[] = "Name";

/**
    The constant representing the __namespace class name
*/
static const char CLASSNAME [] = "__namespace";

void NamespaceProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
        const CIMInstance& myInstance,
	ResponseHandler<CIMObjectPath> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "NamespaceProvider::createInstance()");

	KeyBinding        kb;
        String            keyName;
        String            keyValueString;
       //
       // check if the class name requested is correct
       //
       if (!instanceReference.equalClassName(CLASSNAME))
       {
	   PEG_METHOD_EXIT();
	   throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
				       instanceReference.getClassName());
       }
#ifdef NEVER
       //
       // validate key bindings
       //

       Array<KeyBinding> kbArray = instanceReference.getKeyBindings();
       if ( (kbArray.size() != 1) ||
	    (!String::equalNoCase(kbArray[0].getName(), NAMESPACE_NAME)) )
       //instanceReference.print(cout);
       //myInstance.print(cout);
       {
	   instanceReference.print(cout);
	   PEG_METHOD_EXIT();
	   throw PEGASUS_CIM_EXCEPTION(
	       CIM_ERR_INVALID_PARAMETER,
	       "Invalid instance name "	+
	       ((kbArray.size() != 1) ?
		String("Incorrect number of keys") :
	         String(kbArray[0].getName())));
       }

       // Get the name property from the object
       keyValue.assign(kbArray[0].getValue());
#else
       try
       {
           Uint32 pos = myInstance.findProperty(NAMESPACE_NAME);
           if (pos == PEG_NOT_FOUND)
           {
	      PEG_METHOD_EXIT();
	      throw PropertyNotFound(NAMESPACE_NAME);
           }
           const CIMValue& keyValue = myInstance.getProperty(pos).getValue();
           if (keyValue.getType() != CIMType::STRING)
           {
	      PEG_METHOD_EXIT();
	      throw InvalidParameter("Invalid type for property " 
                                         + String(NAMESPACE_NAME));
           }
           keyValueString = keyValue.toString();
       }
       catch (Exception& e)
       {
	   PEG_METHOD_EXIT();
           throw InvalidParameter("Invalid property " 
                                         + String(NAMESPACE_NAME));
       }
#endif
       Array<String> namespaceNames;
       try
       {
	   namespaceNames = _repository->enumerateNameSpaces();
       }
       catch(Exception& e)
       {
	   PEG_METHOD_EXIT();

	   throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
       }
       // Determine if this namespace exists.
       // Cound not use Contains since this is equalNoCase

       Boolean found = false;
       for(Uint32 i = 0; i < namespaceNames.size(); i++)
       {
	   if(String::equalNoCase(namespaceNames[i], keyValueString))
	   {
	       found = true;
	       continue;
	   }
       }
       // If found, put out error that cannot create twice.
       if(found)
       {
	       PEG_METHOD_EXIT();
	       throw PEGASUS_CIM_EXCEPTION(
		       CIM_ERR_ALREADY_EXISTS,
		       String("Namespace \"") + keyValueString + "\"");
       }

       // begin processing the request
       handler.processing();

       // try to create the new namespace
       try
       {
	   _repository->createNameSpace(keyValueString);
       }
       catch(Exception& e)
       {
           PEG_METHOD_EXIT();
           throw PEGASUS_CIM_EXCEPTION(
                   CIM_ERR_FAILED, e.getMessage());

            //       String("Namespace \"") + keyValueString + "\"" + " creation error");
       }

       handler.deliver(instanceReference);

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return ;

    }

void NamespaceProvider::deleteInstance(
	const OperationContext & context,
        const CIMObjectPath& instanceName,
	ResponseHandler<CIMInstance> & handler)
    {
        CIMValue                userName ;
        String                  namespaceNameStr;
        Array<KeyBinding>       kbArray;

        PEG_METHOD_ENTER(TRC_USER_MANAGER,"UserAuthProvider::deleteInstance");
        // begin processing the request
        handler.processing();

        try
        {
            //
            // Get the namespace name from the instance
            //
            kbArray = instanceName.getKeyBindings();
            for (Uint32 i = 0; i < kbArray.size(); i++)
            {
                if ( kbArray[i].getName() == NAMESPACE_NAME )
                {
                    namespaceNameStr = kbArray[i].getValue();
                }
                else
                {
                    cerr << "What error do I put out here";
                }
            }
        }
        catch ( CIMException &e )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
        if ( !namespaceNameStr.size() )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION (
                CIM_ERR_INVALID_PARAMETER,
                "name property can not be empty.") ;
        }

        Array<String> namespaceNames;
        try
        {

            namespaceNames = _repository->enumerateNameSpaces();
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
        // Determine if this namespace exists.
        // Cound not use Contains since this is equalNoCase
        Boolean found = false;
        for(Uint32 i = 0; i < namespaceNames.size(); i++)
        {
            if(String::equalNoCase(namespaceNames[i], namespaceNameStr))
            {
                found = true;
                continue;
            }
        }

        // If found, put out error that cannot create twice.
        if(!found)
        {
                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_NOT_FOUND,
                        String("Namespace \"") + namespaceNameStr + "\"");
        }
        try
        {
            _repository->deleteNameSpace(namespaceNameStr);
        }
        catch (Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());

        }
        // complete processing the request
        handler.complete();

        PEG_METHOD_EXIT();
        return;

    }

void NamespaceProvider::getInstance(
	const OperationContext & context,
        const CIMObjectPath& instanceName,
	const Uint32 flags,
        const CIMPropertyList& propertyList,
	ResponseHandler<CIMInstance> & handler)
    {
        PEG_METHOD_ENTER(TRC_CONFIG, "NamespaceProvider::getInstance()");

        Array<String>     propertyInfo;
        KeyBinding        kb;
        String            keyName;
        String            keyValue;

        //
        // check if the class name requested is correct
        //
        if (!instanceName.equalClassName(CLASSNAME))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        instanceName.getClassName());
        }

        //
        // validate key bindings
        //
        Array<KeyBinding> kbArray = instanceName.getKeyBindings();
        if ( (kbArray.size() != 1) ||
             (!String::equalNoCase(kbArray[0].getName(), NAMESPACE_NAME)) )
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER,
                "Invalid instance name");
        }

        keyValue.assign(kbArray[0].getValue());
	Array<String> namespaceNames;
        try
        {

	    namespaceNames = _repository->enumerateNameSpaces();
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();

            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
	// Determine if this namespace exists.
	// Cound not use Contains since this is equalNoCase
	Boolean found = false;
	for(Uint32 i = 0; i < namespaceNames.size(); i++)
	{
	    if(String::equalNoCase(namespaceNames[i], keyValue))
	    {
		found = true;
		continue;
	    }
	}

	if(!found)
	{
		PEG_METHOD_EXIT();
		throw PEGASUS_CIM_EXCEPTION(
			CIM_ERR_NOT_FOUND,
			String("Namespace \"") + keyValue + "\"");
	}




        // begin processing the request
        handler.processing();

        //
        // Get values for the property
        //

	/*************************
        try
        {
            _configManager->getPropertyInfo(keyValue, propertyInfo);
        }
        catch (UnrecognizedConfigProperty& ucp)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_FOUND,
                String("Configuration property \"") + keyValue + "\"");
        }
	**************/


	//Set Name of Class
	CIMInstance instance(CLASSNAME);

	//
	// construct the instance
	//
	instance.addProperty(CIMProperty(NAMESPACE_NAME, keyValue));
	//instance.addProperty(CIMProperty(DEFAULT_VALUE, propertyInfo[1]));
	//instance.addProperty(CIMProperty(CURRENT_VALUE, propertyInfo[2]));
	//instance.addProperty(CIMProperty(PLANNED_VALUE, propertyInfo[3]));
	//instance.addProperty(CIMProperty(DYNAMIC_PROPERTY,
	//    Boolean(propertyInfo[4]=="true"?true:false)));

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
        PEG_METHOD_ENTER(TRC_CONFIG, "NamespaceProvider::enumerateInstances()");
        Array<CIMInstance> instanceArray;
        Array<String> propertyNames;

        //
        // check if the class name requested is PG_ConfigSetting
        //
        if (!String::equalNoCase(CLASSNAME, ref.getClassName()))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                                        ref.getClassName());
        }
	Array<String> namespaceNames;

	// begin processing the request
	handler.processing();

        try
        {

	    namespaceNames = _repository->enumerateNameSpaces();

	    // Build the instances. For now simply build the __Namespace instances
	    // Note that for the moment, the only property is name.
            for (Uint32 i = 0; i < namespaceNames.size(); i++)
            {
                Array<String> propertyInfo;

                CIMInstance        instance(CLASSNAME);

                propertyInfo.clear();
		/*
                _configManager->getPropertyInfo(propertyNames[i], propertyInfo);

                Array<KeyBinding> keyBindings;
                keyBindings.append(KeyBinding(PROPERTY_NAME, propertyInfo[0],
                    KeyBinding::STRING));
                CIMObjectPath instanceName(ref.getHost(), ref.getNameSpace(),
                    __NAMESPACE, keyBindings);
                */
                // construct the instance
                instance.addProperty(CIMProperty(NAMESPACE_NAME, namespaceNames[i]));
                //instance.addProperty(CIMProperty(DEFAULT_VALUE, propertyInfo[1]));
                //instance.addProperty(CIMProperty(CURRENT_VALUE, propertyInfo[2]));
                //instance.addProperty(CIMProperty(PLANNED_VALUE, propertyInfo[3]));
                //instance.addProperty(CIMProperty(DYNAMIC_PROPERTY,
                //    Boolean(propertyInfo[4]=="true"?true:false)));
                //namedInstanceArray.append(
                //    CIMNamedInstance(instanceName, instance));
                instanceArray.append(instance);
            }
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }

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
        
	PEG_METHOD_ENTER(TRC_CONFIG,
            "NamespaceProvider::enumerateInstanceNames()");

        Array<CIMObjectPath> instanceRefs;
        Array<String>       propertyNames;
        Array<KeyBinding>   keyBindings;
        KeyBinding          kb;
        String              hostName;

        hostName.assign(System::getHostName());

	const String& className = classReference.getClassName();
	const String& nameSpace = classReference.getNameSpace();

        if (!String::equalNoCase(CLASSNAME, className))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION( CIM_ERR_NOT_SUPPORTED, className );
        }

	// begin processing the request
	handler.processing();

        Array<String> namespaceNames;
        
        // ATTN: Does this throw an exception?
        namespaceNames = _repository->enumerateNameSpaces();
        
	/*
        // Create an instance name from namespace names
        // ATTN: Legal to append to String in Array?
        for (Uint32 i = 0; i < ns.size(); i++)
        {
            instanceName.append("__Namespace.name=\"");
            instanceName[i].append(ns[i]);
            instanceName[i].append("\"");
        }
        
        // Convert to references here so can return references
        CIMObjectPath ref;
        try
        {
            for (Uint32 i = 0; i < instanceName.size(); i++)
            {
                // Convert instance names to References
                instanceRefs.append(instanceName[i]);
            }
        }
        catch(Exception& e)
        {
            // ATTN: Not sure how to handle this error
            cout << "__Namespace Provider Exception ";
            cout << e.getMessage() <<  endl;
        }
	*/
        try
        {

            Uint32 size = namespaceNames.size();

            for (Uint32 i = 0; i < size; i++)
            {
                keyBindings.append(KeyBinding(NAMESPACE_NAME, namespaceNames[i],
                    KeyBinding::STRING));

                //
                // Convert instance names to References
                //
                CIMObjectPath ref(hostName, nameSpace, className, keyBindings);
                instanceRefs.append(ref);
                keyBindings.clear();
            }
        }
        catch(Exception& e)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
        }
	
	handler.deliver(instanceRefs);

	// complete processing the request
	handler.complete();

        PEG_METHOD_EXIT();
    }

PEGASUS_NAMESPACE_END


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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "SampleFamilyProvider.h"
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>


PEGASUS_NAMESPACE_BEGIN

SampleFamilyProvider::SampleFamilyProvider(void)
{
}

SampleFamilyProvider::~SampleFamilyProvider(void)
{
}

CIMInstance _buildPersonDynamicInstance(String& data)
{
    CIMInstance instance("TST_PersonDynamic");
    instance.addProperty(CIMProperty("Name", data));   // key
    return(instance);
}
CIMInstance _buildInstanceLineageDynamic(CIMObjectPath parent, CIMObjectPath child)
{
    // Note that this is a nasty assumption. May be a different namespace. Need
    // to build dynamically
    // This namespacename is used to set the namespace in the references.
    // We should set it dynamically.
    
    //String nameSpace = "root/SampleProvider";
    //String host = System::getHostName();

    CIMName thisClassReference = "TST_PersonDynamic";
    CIMName assocClassName = "TST_LineageDynamic";
    CIMInstance instance(assocClassName);
    //parent.setHost(host);
    //parent.setNameSpace(nameSpace);
    instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
    instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
    return(instance);

}

CIMInstance _buildInstanceLabeledLineageDynamic(CIMObjectPath parent, CIMObjectPath child, String label)
{
    // Note that this is a nasty assumption. May be a different namespace. Need
    // to build dynamically
    // This namespacename is used to set the namespace in the references.
    // We should set it dynamically.
    
    CIMName thisClassReference = "TST_PersonDynamic";
    CIMName assocClassName = "TST_LabeledLineageDynamic";
    CIMInstance instance(assocClassName);
    instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
    instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
    instance.addProperty(CIMProperty("label", label));
    return(instance);

}


void SampleFamilyProvider::initialize(CIMOMHandle & cimom)
{

    CDEBUG ("initialize");
	_cimom = cimom;
    {   
        String nameSpace = "root/SampleProvider";
        /*
        // Create the classes so we have something to build from. This should not have to 
        // be here but we cannot get to repository yet.
        // create default instances
        */
        
        String refClassName = "TST_PersonDynamic";
        String tst_PersonClassName = "TST_Person";

        //CIMClass class1(CIMName(refClassName), CIMName(tst_PersonClassName));
        CIMClass class1(CIMName("TST_PersonDynamic"), CIMName("TST_Person"));
        class1
            .addProperty(CIMProperty (CIMName ("Name"), String())
                .addQualifier(CIMQualifier ( CIMName ("Key"), true)));
        
        _referencedClass = class1;

		XmlWriter::printClassElement(_referencedClass);
		MofWriter::printClassElement(_referencedClass);

        // Create the association class
        CIMClass a1("TST_LineageDynamic");

        // ATTN: Karl - THis generates a value reference with nothing in it for the classname. WHY?
        CIMValue cv = CIMObjectPath();
        CIMProperty p1(CIMName ("parent"), CIMObjectPath(), 0, CIMName(refClassName));
        p1.addQualifier(CIMQualifier(CIMName ("Key"), true));

        CIMProperty p2(CIMName ("child"), CIMObjectPath(),0, CIMName(refClassName));
        p2.addQualifier(CIMQualifier(CIMName ("Key"), true));

        a1
        .addQualifier(CIMQualifier(CIMName ("association"), true))
        .addProperty(CIMProperty(p1))
        .addProperty(CIMProperty(p2));
        _assocClass = a1;

        // ATTN KS the following is just a hack to get a class for the lineagelabeled assoc
        CIMClass c2;
        try
        {
        c2 = _cimom.getClass(
            OperationContext(),
            nameSpace,
            CIMName("TST_LabeledLineageDynamic"),
            false,
            false,
            false,
            CIMPropertyList());
        }
        catch(CIMException& e)
        {
            CDEBUG("Exception hit " << e.getMessage());
            // ATTN: KS 20030303 - Add an exception return here.
        }

        _assocLabeledClass = c2;
        CDEBUG ("Initialize - Association Class built");
		XmlWriter::printClassElement(_assocLabeledClass);
		MofWriter::printClassElement(_assocLabeledClass);

    }
    // Build instances of the referenced class
    Uint32 Father = _instances.size();
    _instances.append(_buildPersonDynamicInstance(String("Father")));
    //CIMObjectPath reference("TST_PersonDynamic.Name=\"Father\"");
    Uint32 Mother = _instances.size();
    _instances.append(_buildPersonDynamicInstance(String("Mother")));
    Uint32 Son1 = _instances.size();
    _instances.append(_buildPersonDynamicInstance(String("Son1")));
    Uint32 Son2 = _instances.size();
    _instances.append(_buildPersonDynamicInstance(String("Son2")));
    Uint32 Daughter1 = _instances.size();
    _instances.append(_buildPersonDynamicInstance(String("Daughter1")));

    CDEBUG ("initialize - referenced Class Instances Built");
    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
        // Create Instance Names
        _instanceNames.append(_instances[i].buildPath(_referencedClass));
    }
    _delay = 3000;
    CDEBUG ("initialize - referenced Class Instance Names Built using buildpath");
    //
    // Now make the instances for the associations
    //
    CDEBUG ("Initialise - Building Assoc Class instances");
    {
        CIMName thisClassReference = "TST_PersonDynamic";
        CIMName assocClassName = "TST_LineageDynamic";
        
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Father],_instanceNames[Son1]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Father],_instanceNames[Son2]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Father],_instanceNames[Daughter1]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Mother],_instanceNames[Son1]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Mother],_instanceNames[Son2]));
        _instancesLineageDynamic.append(
            _buildInstanceLineageDynamic(_instanceNames[Mother],_instanceNames[Daughter1]));
      
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
        {
            // Create Instance Names
            _instanceNamesLineageDynamic.append(_instancesLineageDynamic[i].buildPath(assocClassName));
        }
        CDEBUG("LineageDynamic " << _instancesLineageDynamic.size() << " instances ");

    }
    {
        CIMName thisClassReference = "TST_PersonDynamic";
        CIMName assocLabeledClassName = "TST_LabeledLineageDynamic";
        
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Father],_instanceNames[Son1],String("one")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Father],_instanceNames[Son2],String("two")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Father],_instanceNames[Daughter1],String("three")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Mother],_instanceNames[Son1],String("four")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Mother],_instanceNames[Son2],String("five")));
        _instancesLabeledLineageDynamic.append(
            _buildInstanceLabeledLineageDynamic(_instanceNames[Mother],_instanceNames[Daughter1],String("six")));
      
        for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
        {
            // Create Instance Names
            _instanceNamesLabeledLineageDynamic.append(
                _instancesLabeledLineageDynamic[i].buildPath(assocLabeledClassName));
        }
        CDEBUG("LabeledLineageDynamic " << _instancesLabeledLineageDynamic.size() << " instances ");

    }
}

void SampleFamilyProvider::terminate(void)
{
    
    // TODO Additional cleanup needed
    CDEBUG("Terminate SampleFamilyProvider ");
}

void SampleFamilyProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    // convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();
    CIMName myClass = instanceReference.getClassName();

    if (myClass == CIMName("tst_persondynamic"))
    {
    	// instance index corresponds to reference index
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
    		if(localReference == _instances[i].buildPath(_referencedClass))
    		{
    			// deliver requested instance
    			handler.deliver(_instances[i]);
    			break;
    		}
    	}
    }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		if(localReference == _instances[i].buildPath(_assocClass))
    		{
    			// deliver requested instance
    			handler.deliver(_instancesLineageDynamic[i]);
    			break;
    		}
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		if(localReference == _instances[i].buildPath(_assocLabeledClass))
    		{
    			// deliver requested instance
    			handler.deliver(_instancesLabeledLineageDynamic[i]);
    			break;
    		}
    	}

    }

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    CDEBUG("enumerateInstances");
	/*CIMClass cimClass;
	try
    {
    cimClass = _cimom.getClass(
		OperationContext(),
		classReference.getNameSpace(),
		classReference.getClassName(),
		false,
		false,
		false,
		CIMPropertyList());
    }
    catch(CIMException& e)
    {
        CDEBUG("Exception hit " << e.getMessage());
        // ATTN: KS 20030303 - Add an exception return here.
    }*/
    CIMNamespaceName nameSpace = classReference.getNameSpace();

    //XmlWriter::printClassElement(cimClass);
    //MofWriter::printClassElement(cimClass);

    // begin processing the request
	handler.processing();

    CIMName myClass = classReference.getClassName();

    if (myClass == CIMName("tst_persondynamic"))
    {
        for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
    		// deliver reference
    		handler.deliver(_instances[i]);
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		handler.deliver(_instancesLineageDynamic[i]);
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		// deliver reference
    		handler.deliver(_instancesLabeledLineageDynamic[i]);
    	}

    }

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
	CDEBUG("Enumerate InstanceNames of " << classReference.toString());
    // begin processing the request
	handler.processing();

    CIMName myClass = classReference.getClassName();
    CIMClass cimClass;
	try
    {
    cimClass = _cimom.getClass(
		OperationContext(),
		classReference.getNameSpace(),
		classReference.getClassName(),
		false,
		false,
		false,
		CIMPropertyList());
    }
    catch(CIMException& e)
    {
        CDEBUG("Exception hit " << e.getMessage());
		throw CIMException(CIM_ERR_NOT_FOUND);
    }

    // ATTN: Use the above to check the existence of the class. Note that we use it in only
    // one place for the moement.  Update to cover the others.
    CIMNamespaceName nameSpace = classReference.getNameSpace();

    CDEBUG("EnumerateInstanceNames for class = " << myClass);
    //ATTN: Modify this to dynamically deliver the Names from the instances and hopefully
    // use the same code base as the enumerateinstances 
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            handler.deliver(_instances[i].buildPath(cimClass));
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		handler.deliver(_instancesLineageDynamic[i].buildPath(_assocClass));
    	}

    }
    //ATTN KS This one is in error since we did not build the corresponding instances.
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		handler.deliver(_instancesLabeledLineageDynamic[i].buildPath(_assocLabeledClass));
    	}

    }
    // complete processing the request
    handler.complete();
}

void SampleFamilyProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
    CIMName myClass = instanceReference.getClassName();
	
    CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		myClass,
		instanceReference.getKeyBindings());
	
	// begin processing the request
	handler.processing();
    Boolean instanceFound = false;

    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            if(localReference == _instances[i].buildPath(_referencedClass))
            {
                // overwrite existing instance
                _instances[i] = instanceObject;
                instanceFound = true;
                break;
            }
    	}

     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLineageDynamic[i].buildPath(_assocClass))
            {
                // overwrite existing instance
                _instancesLineageDynamic[i] = instanceObject;
                instanceFound = true;
                break;
            }
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            // overwrite existing instance
            _instancesLabeledLineageDynamic[i] = instanceObject;
            instanceFound = true;
            break;
    	}

    }
	if (!instanceFound)
		throw CIMException(CIM_ERR_NOT_FOUND);

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    CIMName myClass = instanceReference.getClassName();
    
    // convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	
    CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		myClass,
		instanceReference.getKeyBindings());
	
    handler.processing();
	
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            if(localReference == _instances[i].buildPath(_referencedClass))
            {
                throw CIMObjectAlreadyExistsException(
                                      localReference.toString());
            }
    	}
        // add the new instance to the array
        _instances.append(instanceObject);
        _instanceNames.append(instanceReference);

        // deliver the new instance
        handler.deliver(_instanceNames[_instanceNames.size() - 1]);
     }

    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLineageDynamic[i].buildPath(_assocClass))
            {
                throw CIMObjectAlreadyExistsException(
                                      localReference.toString());
            }
    	}
        // add the new instance to the array
        _instancesLineageDynamic.append(instanceObject);
        _instanceNamesLineageDynamic.append(instanceReference);

        // deliver the new instance
        handler.deliver(_instanceNamesLineageDynamic[_instanceNamesLineageDynamic.size() - 1]);

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLabeledLineageDynamic[i].buildPath(_assocLabeledClass))
            {
                throw CIMObjectAlreadyExistsException(
                                      localReference.toString());
            }
    	}
        // add the new instance to the array
        _instancesLabeledLineageDynamic.append(instanceObject);
        _instanceNamesLabeledLineageDynamic.append(instanceReference);

        // deliver the new instance
        handler.deliver(_instanceNamesLabeledLineageDynamic[_instanceNamesLabeledLineageDynamic.size() - 1]);
    }

	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// begin processing the request

    Boolean instanceFound = false;
	handler.processing();
    CIMName myClass = instanceReference.getClassName();
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            if(localReference == _instances[i].buildPath(_referencedClass))
            {
			// remove instance from the array
			_instances.remove(i);
			_instanceNames.remove(i);
            instanceFound = true;
            }
			// exit loop
			break;
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLineageDynamic[i].buildPath(_assocClass))
            {
                // remove instance from the array
                _instancesLineageDynamic.remove(i);
                _instanceNamesLineageDynamic.remove(i);
                instanceFound = true;
            }

			// exit loop
			break;
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            if(localReference == _instancesLabeledLineageDynamic[i].buildPath(_assocLabeledClass))
            {
                // remove instance from the array
                _instancesLabeledLineageDynamic.remove(i);
                _instanceNamesLabeledLineageDynamic.remove(i);
                instanceFound = true;
            }

            // exit loop
            break;
    	}
    }
	if (!instanceFound)
		throw CIMException(CIM_ERR_NOT_FOUND);
	handler.complete();
}

void SampleFamilyProvider::associators(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler)
{
	CDEBUG("Associators");
    // begin processing the request
    // Get the namespace and host names to create the CIMObjectPath

    String host = System::getHostName();
    CIMNamespaceName nameSpace = objectName.getNameSpace();
    CIMName myClass = objectName.getClassName();

    if (!(myClass == CIMName("tst_persondynamic")))
		throw CIMException(CIM_ERR_NOT_FOUND);
        
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
    CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		objectName.getClassName(),
		objectName.getKeyBindings());
    
    handler.processing();
    // Here make the decision between Lineage and LabeledLineage

	// For all of the association objects.
    // This is wrong.  Simply want to deliver something right now.
    Array<CIMObjectPath> outputObjectPaths;
    for(Uint32 i = 0, n = _instanceNamesLineageDynamic.size(); i < n; i++)
	{
        
        CIMInstance instance = _instancesLineageDynamic[i];
        //Array<CIMObjectPath> outputPaths
        CDEBUG("Associators Assoc Class = " << associationClass.getString() << " Role = " << role);
        if (associationClass.isNull() || instance.getClassName().equal(associationClass))
        {
            if (role != String.EMPTY)
            {
                Uint32 pos;
                if ((pos = instance.findProperty(role)) == PEG_NOT_FOUND)
                {
                    throw CIMException(CIM_ERR_INVALID_PARAMETER);
                }
                else     // pos represents property
                {
                    // Does the reference point back to our instance path
                    CIMProperty p = instance.getProperty(pos);
                    if (p.getType() != CIMTYPE_REFERENCE)
                    {
                        throw CIMException(CIM_ERR_INVALID_PARAMETER);
                    }
                    CIMValue v = p.getValue();
                    CIMObjectPath path;
                    v.get(path);
                    // Note that this may be hack to set host names.  Not sure
                    // what is general solution to compare these objects.
                    //path.setHost(host);
                    //path.setNameSpace(nameSpace);
                    
                    if (path.identical(localReference))
                    {
                        // Get other reference properties and deliver them
                        for (Uint32 j = 0; j < instance.getPropertyCount() ; j++)
                        {
                            CIMProperty p = instance.getProperty(j);
                            if (p.getType() == CIMTYPE_REFERENCE && j != pos)
                            {
                                CIMValue v = p.getValue();
                                CIMObjectPath path;
                                v.get(path);
                                outputObjectPaths.append(path);
                            }
                        }
                    }
                }
            }
            else
            {
                for (Uint32 j = 0; j < instance.getPropertyCount() ; j++)
                {
                    CIMProperty p = instance.getProperty(j);
                    if (p.getType() == CIMTYPE_REFERENCE)
                    {
                        CIMValue v = p.getValue();
                        CIMObjectPath path;
                        v.get(path);
                        if (!path.identical(localReference))
                        {
                            outputObjectPaths.append(path);
                        }
                    }
                }
            }
            // Now search the outputpaths found and deliver objects
            CDEBUG("Found " << outputObjectPaths.size() << " objects to return");
            for (Uint32 i = 0 ; i < outputObjectPaths.size(); i++)
            {
                // get instance and send it.
                CIMObjectPath objectName = outputObjectPaths[i];
                CIMName myClass = objectName.getClassName();
                if (myClass == CIMName("tst_persondynamic"))
                {
                    CIMObjectPath localReference = CIMObjectPath(
                        String(),
                        String(),
                        objectName.getClassName(),
                        objectName.getKeyBindings());
                    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
                    {
                        // Deliver matching reference.
                        if(localReference == _instances[i].buildPath(_referencedClass))
                        {
                            // add host and namespace and deliver object
                            CIMObjectPath path = _instances[i].buildPath(_referencedClass);
                            path.setHost(host);
                            path.setNameSpace(nameSpace);
                            _instances[i].setPath(path);
                            CDEBUG("Deliver object with path= " << path.toString());
                            handler.deliver(_instances[i]);
                        }
                    }
    
                }
            }
        }
	}
    
	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::associatorNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	ObjectPathResponseHandler & handler)
{
    // Get the namespace and host names to create the CIMObjectPath
    String host = System::getHostName();
    CIMNamespaceName nameSpace = objectName.getNameSpace();

    CIMObjectPath localReference = CIMObjectPath(
        String(),
        String(),
        objectName.getClassName(),
        objectName.getKeyBindings());

    CDEBUG("Assoc Class = " << resultClass.getString() << " AssocRole = " << role);

    // Get the references list and isolate those interesting. 
    for(Uint32 i = 0, n = _instanceNamesLineageDynamic.size(); i < n; i++)
	{
        
        CIMInstance instance = _instancesLineageDynamic[i];
        
        CDEBUG("AssocNames Assoc Class = " << associationClass.getString() << " Role = " << role);
        if (associationClass.isNull() || instance.getClassName().equal(associationClass))
        {
            if (role != String.EMPTY)
            {
                Uint32 pos;
                if ((pos = instance.findProperty(role)) == PEG_NOT_FOUND)
                {
                    CDEBUG("Invalid Param Exception");
                    throw CIMException(CIM_ERR_INVALID_PARAMETER);
                }
                else     // pos represents property
                {
                    // Does the reference point back to our instance path
                    CDEBUG("Getting Property from pos = " << pos);
                    CIMProperty p = instance.getProperty(pos);
                    if (p.getType() != CIMTYPE_REFERENCE)
                    {
                        throw CIMException(CIM_ERR_INVALID_PARAMETER);
                    }
                    CIMValue v = p.getValue();
                    CIMObjectPath path;
                    v.get(path);
                    // Note that this may be hack to set host names.  Not sure
                    // what is general solution to compare these objects.
                    //path.setHost(host);
                    //path.setNameSpace(nameSpace);

                    CDEBUG("Path found in property is " << path.toString() << " compare to " << localReference);
                    
                    if (path.identical(localReference))
                    {
                        // Get other reference properties and deliver them
                        for (Uint32 j = 0; j < instance.getPropertyCount() ; j++)
                        {
                            CIMProperty p = instance.getProperty(j);
                            CDEBUG("Search for Reference types " << j);
                            if (p.getType() == CIMTYPE_REFERENCE && j != pos)
                            {
                                CIMValue v = p.getValue();
                                CIMObjectPath path;
                                v.get(path);
                                if (path.getHost().size() == 0)
                                    path.setHost(host);

                                if (path.getNameSpace().isNull())
                                    path.setNameSpace(nameSpace);
                                handler.deliver(path);
                            }
                        }
                    }
                }
            }
            else
            {
                for (Uint32 j = 0; j < instance.getPropertyCount() ; j++)
                {
                    CDEBUG("No role Search for Reference types " << j);
                    CIMProperty p = instance.getProperty(j);
                    if (p.getType() == CIMTYPE_REFERENCE)
                    {
                        CIMValue v = p.getValue();
                        CIMObjectPath path;
                        v.get(path);
                        CDEBUG(" AN path output = " << path.toString() << " localReference " << localReference.toString());
                        if (!path.identical(localReference))
                        {
                            CDEBUG("AN path - Deliver");
                            if (path.getHost().size() == 0)
                                path.setHost(host);

                            if (path.getNameSpace().isNull())
                                path.setNameSpace(nameSpace);
                            handler.deliver(path);
                        }
                    }
                }
            }
        }
    }
	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::references(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler)
{
	CDEBUG("references");
    // Get the namespace and host names to create the CIMObjectPath
    //String nameSpace = "root/SampleProvider";
    String host = System::getHostName();

    CIMNamespaceName nameSpace = objectName.getNameSpace();

    CIMObjectPath localReference = CIMObjectPath(
        String(),
        String(),
        objectName.getClassName(),
        objectName.getKeyBindings());

    handler.processing();

    // Here make the decision between Lineage and LabeledLineage
    CIMName objectClassName = objectName.getClassName();

	// For all of the corresponding association objects.
    for(Uint32 i = 0, n = _instanceNamesLineageDynamic.size(); i < n; i++)
	{
        // Get the other side of the association. How do we know which is other side?

        CIMInstance instance = _instancesLineageDynamic[i];
        CIMObjectPath objectPath =  instance.buildPath(_assocClass);

        CDEBUG("References Result Class = " << resultClass.getString() << " Role = " << role);
        if (resultClass.isNull() || instance.getClassName().equal(resultClass))
        {
            CDEBUG("Passed resultClassTest");
            if (role != String.EMPTY)
            {
                CDEBUG("Role not empty");
                Uint32 pos;
                if ((pos = instance.findProperty(role)) == PEG_NOT_FOUND)
                {
                    CDEBUG("Role exception");
                    throw CIMException(CIM_ERR_INVALID_PARAMETER);
                }
                else     // pos represents property
                {
                    CDEBUG("Has role");
                    // Does the reference point back to our instance path
                    CIMProperty p = instance.getProperty(pos);
                    if (p.getType() != CIMTYPE_REFERENCE)
                    {
                        CDEBUG("p.getType Exception");
                        throw CIMException(CIM_ERR_INVALID_PARAMETER);
                    }
                    CIMValue v = p.getValue();
                    CIMObjectPath path;
                    v.get(path);
                    // Note that this may be hack to set host names.  Not sure
                    // what is general solution to compare these objects.
                    path.setHost(host);
                    path.setNameSpace(nameSpace);
                    CDEBUG("path test path= " << path << " ObjectName " << objectName);
                    if (path.identical(objectName))
                    {
                        if (objectPath.getHost().size() == 0)
                            objectPath.setHost(host);

                        if (objectPath.getNameSpace().isNull())
                            objectPath.setNameSpace(nameSpace);
                        instance.setPath(objectPath);
                        handler.deliver(instance);
                    }
                }
            }
            else
            {
                if (objectPath.getHost().size() == 0)
                    objectPath.setHost(host);

                if (objectPath.getNameSpace().isNull())
                    objectPath.setNameSpace(nameSpace);
                instance.setPath(objectPath);
                handler.deliver(instance);

            }
        }
	}
    
	// complete processing the request
	handler.complete();
}

// Return all references(association instance names) in which the give
// objecdt is involved.

void SampleFamilyProvider::referenceNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	ObjectPathResponseHandler & handler)
{
	CDEBUG("ReferenceNames Operation");
    
    // Get the namespace and host names to create the CIMObjectPath
    CIMNamespaceName nameSpace = objectName.getNameSpace().getString();
    String host = System::getHostName();
    
    CIMObjectPath localReference = CIMObjectPath(
        String(),
        String(),
        objectName.getClassName(),
        objectName.getKeyBindings());


	// For all of the association objects.
    CDEBUG("resultClass= " << ((resultClass.isNull())? "NULL" : resultClass) << " Role = " << ((role == String.EMPTY)? "EMPTY" : role));
    for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
	{
        CIMInstance instance = _instancesLineageDynamic[i];
        CIMObjectPath objectPath =  instance.buildPath(_assocClass);

        if (resultClass.isNull() || objectPath.getClassName().equal(resultClass))
        {
            CDEBUG("Passed result Class Test");
            if (role != String.EMPTY)
            {
                Uint32 pos;
                if ((pos = instance.findProperty(role)) == PEG_NOT_FOUND)
                {
                    CDEBUG("Invalid role parameter");
                    throw CIMException(CIM_ERR_INVALID_PARAMETER);
                }
                else     // pos represents property
                {
                    // Does the reference point back to our instance path
                    CDEBUG("Get Property");
                    CIMProperty p = instance.getProperty(pos);
                    if (p.getType() != CIMTYPE_REFERENCE)
                    {
                        throw CIMException(CIM_ERR_INVALID_PARAMETER);
                    }
                    CIMValue v = p.getValue();
                    CIMObjectPath path;
                    v.get(path);
                    CDEBUG("Get Property path" << path);
                    // Note that this may be hack to set host names.  Not sure
                    // what is general solution to compare these objects.
                    //path.setHost(host);
                    //path.setNameSpace(nameSpace);
                    CDEBUG("Path Compare path= " << path << " objectName " << objectName);
                    if (path.identical(localReference))
                    {
                        if (objectPath.getHost().size() == 0)
                            objectPath.setHost(host);

                        if (objectPath.getNameSpace().isNull())
                            objectPath.setNameSpace(nameSpace);

                        handler.deliver(objectPath);
                    }
                }
            }
            else
            {
                CDEBUG("Deliver object path " << objectPath.toString());
                if (objectPath.getHost().size() == 0)
                    objectPath.setHost(host);

                if (objectPath.getNameSpace().isNull())
                    objectPath.setNameSpace(nameSpace);
                handler.deliver(objectPath);

            }
        }
	}
    
	// complete processing the request
	handler.complete();
}


PEGASUS_NAMESPACE_END

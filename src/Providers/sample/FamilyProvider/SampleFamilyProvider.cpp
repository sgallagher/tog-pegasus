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

void SampleFamilyProvider::initialize(CIMOMHandle & cimom)
{

    CDEBUG ("initialize");
	_cimom = cimom;
    {   /*
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
        CIMClass a1("TST_Lineage");

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

        _assocLabeledClass = a1;
        CDEBUG ("Initialize - Association Class built");
		//XmlWriter::printClassElement(_assocClass);
		//MofWriter::printClassElement(_assocClass);

    }
    // Build instances of the referenced class
    {
    	CIMInstance instance("TST_PersonDynamic");
    	//CIMObjectPath reference("TST_PersonDynamic.Name=\"Father\"");
    
    	instance.addProperty(CIMProperty("Name", String("Father")));   // key
    
    	_instances.append(instance);
    	//_instanceNames.append(reference);
    }
    Uint32 Father = _instances.size() -1;
    {
    	CIMInstance instance("TST_PersonDynamic");
    	//CIMObjectPath reference("TST_PersonDynamic.Name=\"Father\"");
    
    	instance.addProperty(CIMProperty("Name", String("Mother")));   // key
    
    	_instances.append(instance);
    	//_instanceNames.append(reference);
    }
    Uint32 Mother = _instances.size() -1;
    {
    	CIMInstance instance("TST_PersonDynamic");
    	//CIMObjectPath reference("TST_PersonDynamic.Name=\"Son1\"");
    
    	instance.addProperty(CIMProperty("Name", String("Son1")));   // key
    
    	_instances.append(instance);
    	//_instanceNames.append(reference);
    }
    Uint32 Son1 = _instances.size() -1;
    {
    	CIMInstance instance("TST_PersonDynamic");
    	//CIMObjectPath reference("TST_PersonDynamic.Name=\"Son2\"");
    
    	instance.addProperty(CIMProperty("Name", String("Son2")));   // key
    
    	_instances.append(instance);
    	//_instanceNames.append(reference);
    }
    Uint32 Son2 = _instances.size() - 1;
    {
    	CIMInstance instance("TST_PersonDynamic");
    	//CIMObjectPath reference("TST_PersonDynamic.Name=\"Daughter1\"");
    
    	instance.addProperty(CIMProperty("Name", String("Daughter1")));   // key
    
    	_instances.append(instance);
    	//_instanceNames.append(reference);
    }
    Uint32 Daughter1 = _instances.size() - 1;

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
        // Note that this is a nasty assumption. May be a different namespace. Need
        // to build dynamically
        // This namespacename is used to set the namespace in the references.
        // We should set it dynamically.
        String nameSpace = "SampleProvider";
        String host = System::getHostName();
        
        CIMName thisClassReference = "TST_PersonDynamic";
        CIMName assocClassName = "TST_LineageDynamic";
        {
            CIMInstance instance(assocClassName);
            CIMObjectPath parent =  _instanceNames[Father];
            parent.setHost(host);
            parent.setNameSpace(nameSpace);
            CIMObjectPath child  = _instanceNames[Son1];
            instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
            instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
            _instancesLineageDynamic.append(instance);
        }
        {
            CIMInstance instance(assocClassName);
            CIMObjectPath parent =  _instanceNames[Father];
            parent.setHost(host);
            parent.setNameSpace(nameSpace);
            CIMObjectPath child = _instanceNames[Son2];
            child.setHost(host);
            child.setNameSpace(nameSpace);
        
            instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
            instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
            _instancesLineageDynamic.append(instance);
        }
        {
            CIMInstance instance(assocClassName);
            CIMObjectPath parent =  _instanceNames[Father];
            parent.setHost(host);
            parent.setNameSpace(nameSpace);
            CIMObjectPath child = _instanceNames[Daughter1];
            child.setHost(host);
            child.setNameSpace(nameSpace);
        
            instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
            instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
            _instancesLineageDynamic.append(instance);
        }
        {
            CIMInstance instance(assocClassName);
            CIMObjectPath parent =  _instanceNames[Mother];
            parent.setHost(host);
            parent.setNameSpace(nameSpace);
            CIMObjectPath child = _instanceNames[Daughter1];
            child.setHost(host);
            child.setNameSpace(nameSpace);
        
            instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
            instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
            _instancesLineageDynamic.append(instance);
        }
        {
            CIMInstance instance(assocClassName);
            CIMObjectPath parent =  _instanceNames[Mother];
            parent.setHost(host);
            parent.setNameSpace(nameSpace);
            CIMObjectPath child = _instanceNames[Son1];
            child.setHost(host);
            child.setNameSpace(nameSpace);
        
            instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
            instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
            _instancesLineageDynamic.append(instance);
        }
        {
            CIMInstance instance(assocClassName);
            CIMObjectPath parent =  _instanceNames[Mother];
            parent.setHost(host);
            parent.setNameSpace(nameSpace);
            CIMObjectPath child = _instanceNames[Son2];
            child.setHost(host);
            child.setNameSpace(nameSpace);
        
            instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
            instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
            _instancesLineageDynamic.append(instance);
        }
        for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
        {
            // Create Instance Names
            _instanceNamesLineageDynamic.append(_instancesLineageDynamic[i].buildPath(assocClassName));
        }

    }
    CDEBUG("LineageDynamic " << _instancesLineageDynamic.size() << " instances ");
    // From the enumerateinstancenames  Why the R
    // TST_Lineage.child=R"Person.name=\"Sofi\"",parent=R"Person.name=\"Mike\""
    
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

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		if(localReference == _instances[i].buildPath(_referencedClass))
		// if(localReference == _instanceNames[i])
		{
			// deliver requested instance
			handler.deliver(_instances[i]);
			break;
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
        // ATTN: KS 20030303 - Add an exception return here.
    }

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
    CDEBUG("timer = " << _delay);
    pegasus_sleep(_delay);
    _delay += 10000;
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
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// begin processing the request
	handler.processing();

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		//if(localReference == _instanceNames[i])
        if(localReference == _instances[i].buildPath(_referencedClass))
		{
			// overwrite existing instance
			_instances[i] = instanceObject;
			
			break;
		}
	}
	
	// complete processing the request
	handler.complete();
}

void SampleFamilyProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    // convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());
	
	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			throw CIMObjectAlreadyExistsException(
                                  localReference.toString());
		}
	}
			
	// begin processing the request
	handler.processing();

	// add the new instance to the array
	_instances.append(instanceObject);
	_instanceNames.append(instanceReference);

	// deliver the new instance
	handler.deliver(_instanceNames[_instanceNames.size() - 1]);

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
	handler.processing();
    CIMName myClass = instanceReference.getClassName();
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
			// save the instance locally
			CIMInstance cimInstance(_instances[i]);

			// remove instance from the array
			_instances.remove(i);
			_instanceNames.remove(i);

			// exit loop
			break;
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
			// save the instance locally
			CIMInstance cimInstance(_instancesLineageDynamic[i]);

			// remove instance from the array
			_instancesLineageDynamic.remove(i);
			_instanceNamesLineageDynamic.remove(i);

			// exit loop
			break;
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
            // save the instance locally
            CIMInstance cimInstance(_instancesLineageDynamic[i]);

            // remove instance from the array
            _instancesLabeledLineageDynamic.remove(i);
            _instanceNamesLabeledLineageDynamic.remove(i);

            // exit loop
            break;
    	}

    }
	handler.complete();
    /*
	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// save the instance locally
			CIMInstance cimInstance(_instances[i]);

			// remove instance from the array
			_instances.remove(i);
			_instanceNames.remove(i);

			// exit loop
			break;
		}
	}
	**** Delete this whole thing*/
	// complete processing the request
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

    String nameSpace = "SampleProvider";
    String host = System::getHostName();

    handler.processing();
    // Here make the decision between Lineage and LabeledLineage

	// For all of the association objects.
    // This is wrong.  Simply want to deliver something right now.
    for(Uint32 i = 0, n = _instanceNamesLineageDynamic.size(); i < n; i++)
	{
        /*
        The AssocClass input parameter, if not NULL, MUST be a valid CIM 
        Association Class name.  It acts as a filter on the returned set of 
        Objects by mandating that each returned Object MUST be associated to the 
        source Object via an Instance of this Class or one of its subclasses.
  
        The ResultClass input parameter, if not NULL, MUST be a valid CIM Class 
        name.  It acts as a filter on the returned set of Objects by mandating 
        that each returned Object MUST be either an Instance of this Class (or one 
        of its subclasses) or be this Class (or one of its subclasses).  

        The Role input parameter, if not NULL, MUST be a valid Property name.  It 
        acts as a filter on the returned set of Objects by mandating that each 
        returned Object MUST be associated to the source Object via an Association 
        in which the source Object plays the specified role (i.e.  the name of the 
        Property in the Association Class that refers to the source Object MUST 
        match the value of this parameter).  

        The ResultRole input parameter, if not NULL, MUST be a valid Property 
        name.  It acts as a filter on the returned set of Objects by mandating 
        that each returned Object MUST be associated to the source Object via an 
        Association in which the returned Object plays the specified role (i.e.  
        the name of the Property in the Association Class that refers to the 
        returned Object MUST match the value of this parameter).  
        
        If the IncludeQualifiers input parameter is true, this specifies that all 
        Qualifiers for each Object (including Qualifiers on the Object and on any 
        returned Properties) MUST be included as <QUALIFIER> elements in the 
        response.  If false no <QUALIFIER> elements are present in each returned 
        Object.  
        
        If the IncludeClassOrigin input parameter is true, this specifies that the 
        CLASSORIGIN attribute MUST be present on all appropriate elements in each 
        returned Object.  If false, no CLASSORIGIN attributes are present in each 
        returned Object.
        */  
        
    // Filter out by resultClass and role.
        // The ResultClass input parameter, if not NULL, MUST be a valid CIM Class name.
        // It acts as a filter on the returned set of Object Names by mandating that each
        // returned Object Name MUST identify an Instance of this Class (or one of its subclasses),
        // or this Class (or one of its subclasses). 

        // The Role input parameter, if not NULL, MUST be a valid Property name. It acts as a
        // filter on the returned set of Object Names by mandating that each returned Object Name
        // MUST identify an Object that refers to the target Instance via a Property whose name
        // matches the value of this parameter. 
        
        // Note that here we test to determine if the returned object name equals resultClass
        // or any of its subclasses
        
        // NOTE: THis code is just a hack today to return objects so we can test the internal
        // paths.  In fact, it just returns the associations, not the corresponding objects from
        // the association.
        // ATTN: Fix this KS.
        CIMInstance instance = _instancesLineageDynamic[i];
        // Filter out associations that do not match the association class
        if (instance.getClassName().equal(associationClass))
        {
            CDEBUG("FamilyProvider AssociatorsResult Class = " << resultClass.getString()<< " Role = " << role);
            if (resultClass.isNull() || instance.getClassName().equal(resultClass))
            {
                // Incomplete.  Need to add the other filters.
                CIMObjectPath objectPathInput = objectName;
                if (objectPathInput.getHost().size() == 0)
                    objectPathInput.setHost(host);
    
                if (objectPathInput.getNameSpace().isNull())
                    objectPathInput.setNameSpace(nameSpace);
                CIMObject cimObject(instance);
                cimObject.setPath (objectPathInput);
    
                // set path from CIMObjectPath containing the path.
                handler.deliver(cimObject);
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
    String nameSpace = "SampleProvider";
    String host = System::getHostName();
    // ATTN: Just a hack to get objects back. Note that today it returns the
    // association class, not the corresponding 
    CDEBUG("Result Class = " << resultClass.getString() << " Role = " << role);
    for(Uint32 i = 0, n = _instanceNamesLineageDynamic.size(); i < n; i++)
	{
        // Filter out by resultClass and role.
        // The ResultClass input parameter, if not NULL, MUST be a valid CIM Class name.
        // It acts as a filter on the returned set of Object Names by mandating that each
        // returned Object Name MUST identify an Instance of this Class (or one of its subclasses),
        // or this Class (or one of its subclasses). 

        // The Role input parameter, if not NULL, MUST be a valid Property name. It acts as a
        // filter on the returned set of Object Names by mandating that each returned Object Name
        // MUST identify an Object that refers to the target Instance via a Property whose name
        // matches the value of this parameter. 
        
        // Note that here we test to determine if the returned object name equals resultClass
        // or any of its subclasses
        
        CIMObjectPath r = _instanceNamesLineageDynamic[i];

        if (resultClass.isNull() || r.getClassName().equal(resultClass))
        {
            CDEBUG("Sending AssociatorNameResponse");
            if (r.getHost().size() == 0)
                r.setHost(host);
    
            if (r.getNameSpace().isNull())
                r.setNameSpace(nameSpace);
        }
        handler.deliver(r);
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
    // begin processing the request
    // Get the namespace and host names to create the CIMObjectPath
    String nameSpace = "SampleProvider";
    String host = System::getHostName();

    handler.processing();
    // Here make the decision between Lineage and LabeledLineage
    CIMName objectClassName = objectName.getClassName();

	// For all of the corresponding association objects.
    // NOTE: This is just a hack today to return objects so we can get paths running
    for(Uint32 i = 0, n = _instanceNamesLineageDynamic.size(); i < n; i++)
	{
        // Filter out by resultClass and role.
        // The ResultClass input parameter, if not NULL, MUST be a valid CIM Class name.
        // It acts as a filter on the returned set of Object Names by mandating that each
        // returned Object Name MUST identify an Instance of this Class (or one of its subclasses),
        // or this Class (or one of its subclasses). 

        // The Role input parameter, if not NULL, MUST be a valid Property name. It acts as a
        // filter on the returned set of Object Names by mandating that each returned Object Name
        // MUST identify an Object that refers to the target Instance via a Property whose name
        // matches the value of this parameter. 
        
        // Note that here we test to determine if the returned object name equals resultClass
        // or any of its subclasses
        
        CIMInstance instance = _instancesLineageDynamic[i];

        CDEBUG("References Result Class = " << resultClass.getString() << " Role = " << role);
        if (resultClass.isNull() || instance.getClassName().equal(resultClass))
        {
            // Incomplete.  Need to add the other filters.
            CIMObjectPath objectPathInput = objectName;
            if (objectPathInput.getHost().size() == 0)
                objectPathInput.setHost(host);

            if (objectPathInput.getNameSpace().isNull())
                objectPathInput.setNameSpace(nameSpace);
            CIMObject cimObject(instance);
            cimObject.setPath (objectPathInput);
            handler.deliver(cimObject);
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
    String nameSpace = objectName.getNameSpace().getString();
    String host = System::getHostName();

    CIMName myClass = objectName.getClassName();
    CIMClass cimClass;
	try
    {
    cimClass = _cimom.getClass(
		OperationContext(),
		objectName.getNameSpace(),
		objectName.getClassName(),
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

	// For all of the association objects.
    CDEBUG("resultClass = " << ((resultClass.isNull())? "NULL" : resultClass));
    for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
	{
        // Filter out by resultClass and role.
        // The ResultClass input parameter, if not NULL, MUST be a valid CIM Class name.
        // It acts as a filter on the returned set of Object Names by mandating that each
        // returned Object Name MUST identify an Instance of this Class (or one of its subclasses),
        // or this Class (or one of its subclasses). 

        // The Role input parameter, if not NULL, MUST be a valid Property name. It acts as a
        // filter on the returned set of Object Names by mandating that each returned Object Name
        // MUST identify an Object that refers to the target Instance via a Property whose name
        // matches the value of this parameter. 
        
        // Note that here we test to determine if the returned object name equals resultClass
        // or any of its subclasses
        //CIMObjectPath objectPath = _instanceNamesLineageDynamic[i];
        // ATTN: Need to dynamically get the assocClass and also put a try block around the following
        // code.
        CIMObjectPath objectPath =  _instancesLineageDynamic[i].buildPath(_assocClass);
        if (resultClass.isNull() || objectPath.getClassName().equal(resultClass))
        {
            if (objectPath.getHost().size() == 0)
                objectPath.setHost(host);
    
            if (objectPath.getNameSpace().isNull())
                objectPath.setNameSpace(nameSpace);
            handler.deliver(objectPath);
        }
	}
    
	// complete processing the request
	handler.complete();
}


PEGASUS_NAMESPACE_END

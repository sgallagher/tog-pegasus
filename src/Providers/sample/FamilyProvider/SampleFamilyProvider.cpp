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
#include <Pegasus/Common/Tracer.h>
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
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::initialize");

    CDEBUG ("initialize");
    {   /*
        // Create the classes so we have something to build from. This should not have to 
        // be here but we cannot get to repository yet.
        // create default instances
        */
        String refClassName = "TST_PersonDynamic";
        String tst_PersonClassName = "TST_Person";

        //CIMClass class1(CIMName(refClassName), CIMName(tst_PersonClassName));
        CIMClass class1(CIMName(refClassName), CIMName("TST_Person"));
        /*
	    CIMProperty prop1 = CIMProperty (CIMName ("Name"), String());
        prop1.addQualifier (CIMQualifier (CIMName ("Key"), true));
        class1.addProperty(prop1);
        */
        class1
            .addProperty(CIMProperty (CIMName ("Name"), String())
                .addQualifier(CIMQualifier ( CIMName ("Key"), true)));
        //
	    //class1.addProperty(prop1);
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
        CDEBUG ("Initialize - Association Class built");
		XmlWriter::printClassElement(_assocClass);
		MofWriter::printClassElement(_assocClass);

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
    CDEBUG ("initialize - referenced Class Instance Names Built using buildpath");
    //
    // Now make the instances for the associations
    //
    
    /* Lets go to the path stuff rather than the separate referece object.
    instance0.setPath(CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));
    assert(instance0.getPath() == CIMObjectPath("//localhost/root/cimv2:MyClass.Foo=1"));
    
	CIMObjectPath r1 ("atp:77", CIMNamespaceName ("root/cimv25"), 
            CIMName ("TennisPlayer"));
    
    */
    CDEBUG ("Initialise - Building Assoc Class instances");
    {
        // Note that this is a nasty assumption. May be a different namespace. Need
        // to build dynamically
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
    
    /*String myParent = "//localhost/root/SampleProvider:TST_PersonDynamic.Name=\"Father\"";
    String myChild = "//localhost/root/SampleProvider:TST_PersonDynamic.Name=\"Daughter1\"";
    CIMName className = "TST_LineageDynamic";
    CIMInstance instance(className);
    CIMObjectPath parent =  myParent;
    CIMObjectPath child = myChild;
    CIMName thisClassReference = "TST_PersonDynamic";  
    instance.addProperty(CIMProperty("parent", parent,0,thisClassReference));
    instance.addProperty(CIMProperty("child", child, 0, thisClassReference));
    CDEBUG ("initialize - Assoc Class Instance Built");
    */
    //CIMObjectPath reference = "//localhost/root/SampleProvider:TST_LineageDynamic.parent=\"TST_PersonDynamic.name=\"Father\"\",Child=\"TST_PersonDynamic.name=\"Daughter1\"\"";
    
    //Array <CIMKeyBinding> keyBindings;
    /*
    CIMKeyBinding aBinding ("a", "A.y=\"lavender\",x=\"rose\",z=\"rosemary\"", 
        CIMKeyBinding::REFERENCE);
    CIMKeyBinding bBinding ("b", "B.s=\"sage\",q=\"pelargonium\",r=\"tyme\"",
        CIMKeyBinding::REFERENCE);
    keyBindings.append (aBinding);
    keyBindings.append (bBinding);

    CIMObjectPath cPath2 ("", CIMNamespaceName (),
        cPath.getClassName (), keyBindings);
        
    CIMObjectPath aPath = instanceA.buildPath (classA);
    
        CIMKeyBinding aBinding ("a", "TST_PersonDynamic.Name=\"Father\"", 
        CIMKeyBinding::REFERENCE);
    */
    //CIMObjectPath reference instance.buildPath( className );  
 // CIMObjectPath reference("//localhost/root/SampleProvider:TST_LineageDynamic.parent=TST_PersonDynamic.name=\"Father\",Child=TST_PersonDynamic.name=\"Daughter1\"");
    /*String referenceString = "TST_Lineage";
    referenceString.append(".parent=\"");
    referenceString.append(myParent);
    referenceString.append("\"");
    referenceString.append(",child=\"");
    referenceString.append(myChild);
    referenceString.append("\"");
    PEGASUS_STD(cout) << "KSTEST ASSOC String = " << referenceString << PEGASUS_STD(endl);
    //CIMObjectPath reference =  referenceString;

    
    CDEBUG ("initialize - 3");
        
    _instancesLineage.append(instance);
    //_instanceNamesLineage.append(reference);
    }*/
    CDEBUG ("initialize - 4 ");
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::terminate(void)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::terminate");
    
    // TODO Mike Day mentioned additional cleanup needed
    PEGASUS_STD(cout) << "KSTEST Terminate SampleFamilyProvider " << PEGASUS_STD(endl);
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
      "SampleFamilyProvider::getInstance");
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
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::enumerateInstances");
    PEGASUS_STD(cout) << "KSTEST enumerateInstances SampleFamilyProvider "  << PEGASUS_STD(endl);
	
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
    	PEGASUS_STD(cout) << "KSTEST tst_Lineagedynamic instances " << _instancesLineageDynamic.size() << PEGASUS_STD(endl);
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
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::enumerateInstanceNames");
	PEGASUS_STD(cout) << "KSTEST Enumerate InstanceNames of " << classReference.toString() << PEGASUS_STD(endl);
    // begin processing the request
	handler.processing();

    CIMName myClass = classReference.getClassName();

    //ATTN: Modify this to dynamically deliver the Names from the instances and hopefully
    // use the same code base as the enumerateinstances 
    if (myClass == CIMName("tst_persondynamic"))
    {
    	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    	{
            handler.deliver(_instances[i].buildPath(_referencedClass));
    	}
     }
    if (myClass == CIMName("tst_lineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLineageDynamic.size(); i < n; i++)
    	{
    		handler.deliver(_instanceNamesLineageDynamic[i]);
    	}

    }
    if (myClass ==  CIMName("tst_labeledlineagedynamic"))
    {
    	for(Uint32 i = 0, n = _instancesLabeledLineageDynamic.size(); i < n; i++)
    	{
    		handler.deliver(_instanceNamesLabeledLineageDynamic[i]);
    	}

    }
    // complete processing the request
    handler.complete();
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::modifyInstance");
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
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::createInstance");
	
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
    PEG_METHOD_EXIT();
}

void SampleFamilyProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::deleteInstance");
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
	handler.complete();
    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::associators");
	PEGASUS_STD(cout) << "KSTEST Associators SampleFamilyProvider" << PEGASUS_STD(endl);
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
        PEGASUS_STD(cout) << "KSTEST Result Class = " << resultClass.getString() 
            << " Role = " << role
            << PEGASUS_STD(endl);
        if (resultClass.isNull() || instance.getClassName().equal(resultClass))
        {
            // Incomplete.  Need to add the other filters.
            handler.deliver(instance);
        }
	}
    
	// complete processing the request
	handler.complete();
    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::associatorNames");
	throw CIMNotSupportedException("SampleFamilyProvider::associatorNames");
    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::references");
	PEGASUS_STD(cout) << "KSTESTreferences SampleFamilyProvider" << PEGASUS_STD(endl);
    // begin processing the request
    // Get the namespace and host names to create the CIMObjectPath
    String nameSpace = "SampleProvider";
    String host = System::getHostName();

    handler.processing();
    // Here make the decision between Lineage and LabeledLineage

	// For all of the association objects.
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
        PEGASUS_STD(cout) << "KSTEST Result Class = " << resultClass.getString() 
            << " Role = " << role
            << PEGASUS_STD(endl);
        if (resultClass.isNull() || instance.getClassName().equal(resultClass))
        {
            // Incomplete.  Need to add the other filters.
            handler.deliver(instance);
        }
	}
    
	// complete processing the request
	handler.complete();
    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(TRC_PROVIDER,
       "SampleFamilyProvider::referenceNames");
	PEGASUS_STD(cout) << "KSTESTreferenceNames SampleFamilyProvider" << PEGASUS_STD(endl);
    
    
    // Get the namespace and host names to create the CIMObjectPath
    String nameSpace = "SampleProvider";
    String host = System::getHostName();


	// For all of the association objects.
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
        PEGASUS_STD(cout) << "KSTEST Result Class = " << resultClass.getString() 
            << " Role = " << role
            << PEGASUS_STD(endl);
        if (resultClass.isNull() || r.getClassName().equal(resultClass))
        {
            if (r.getHost().size() == 0)
                r.setHost(host);
    
            if (r.getNameSpace().isNull())
                r.setNameSpace(nameSpace);

            PEGASUS_STD(cout) << "KSTEST Deliver CIMOBjectPath = " << r.toString() << PEGASUS_STD(endl);
         handler.deliver(r);
        }
        
	}
    
	// complete processing the request
	handler.complete();
    PEG_METHOD_EXIT();
}


PEGASUS_NAMESPACE_END

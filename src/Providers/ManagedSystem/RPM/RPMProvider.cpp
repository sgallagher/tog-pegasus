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
// Author: Tim Potter <tpot@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Provider/CIMInstanceProvider.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#include <fcntl.h>
#include <syslog.h>

#include "RPMProvider.h"
#include "RPMProviderUtils.h"

RPMProvider::RPMProvider()
{
}

RPMProvider::~RPMProvider()
{
}

void RPMProvider::initialize(CIMOMHandle &handle)
{
    // Initialise RPM database

    if (rpmReadConfigFiles(NULL, NULL) != 0)
        throw CIMException(CIM_ERR_FAILED, "rpmReadConfigFiles() failed");

    if (rpmdbOpen(NULL, &_db, O_RDONLY, 0644) != 0)
        throw CIMException(CIM_ERR_FAILED, "rpmdbOpen() failed");
}

void RPMProvider::terminate()
{
    // Clean up RPM database

    rpmdbClose(_db);

    delete this;
}

// Return arrays of package objects

Array<CIMInstance> 
RPMProvider::enumeratePackageInstances()
{
    Array<CIMInstance> instances;

    IterWrapper iter(rpmdbInitIterator(_db, RPMTAG_NAME, NULL, 0));

    if (iter.isNull())
	return instances;

    for (Header h = rpmdbNextIterator(iter); h; 
	 h = Header(rpmdbNextIterator(iter))) {

	instances.append(packageInstanceFromHeader(h));
    }

    return instances;
}

Array<CIMObjectPath> 
RPMProvider::enumeratePackageInstanceNames()
{
    Array<CIMObjectPath> paths;

    IterWrapper iter(rpmdbInitIterator(_db, RPMTAG_NAME, NULL, 0));

    if (iter.isNull())
	return paths;

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	paths.append(packageObjectPathFromHeader(h));
    }
	
    return paths;
}

// Return arrays of file objects

Array<CIMInstance> 
RPMProvider::enumerateFileInstances()
{
    Array<CIMInstance> instances;

    IterWrapper iter(rpmdbInitIterator(_db, RPMTAG_BASENAMES, NULL, 0));

    if (iter.isNull())
	return instances;

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	Array<CIMInstance> files(fileInstancesFromHeader(h));
	
	for (Uint32 i = 0; i < files.size(); i++)
	    instances.append(files[i]);
    }
	
    return instances;
}

Array<CIMObjectPath> 
RPMProvider::enumerateFileInstanceNames()
{
    Array<CIMObjectPath> paths;

    IterWrapper iter(rpmdbInitIterator(_db, RPMTAG_BASENAMES, NULL, 0));

    if (iter.isNull())
	return paths;

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	Array<CIMObjectPath> files(fileObjectPathsFromHeader(h));

	for (Uint32 i = 0; i < files.size(); i++) 
	    paths.append(files[i]);
    }

    return paths;
}

// Return arrays of product objects

Array<CIMInstance> 
RPMProvider::enumerateProductInstances()
{
    Array<CIMInstance> instances;

    IterWrapper iter (rpmdbInitIterator(_db, RPMTAG_NAME, NULL, 0));

    if (iter.isNull())
	return instances;

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	instances.append(productInstanceFromHeader(h));
    }

    return instances;
}

Array<CIMObjectPath> 
RPMProvider::enumerateProductInstanceNames()
{
    Array<CIMObjectPath> paths;

    IterWrapper iter(rpmdbInitIterator(_db, RPMTAG_NAME, NULL, 0));

    if (iter.isNull())
	return paths;

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	paths.append(productObjectPathFromHeader(h));
    }

    return paths;
}

// Return the value of the string Name property for an object reference

static String
nameFromReference(const CIMObjectPath &ref)
{
    Array<CIMKeyBinding> kb(ref.getKeyBindings());

    for (Uint32 i = 0; i < kb.size(); i++) {
	if (kb[i].getName() == "Name") {
	    if (kb[i].getType() == CIMKeyBinding::STRING) {
		return kb[i].getValue();
	    }
	}
    }

    return String::EMPTY;
}

// getInstance helpers

void
RPMProvider::getPackageInstance(const OperationContext &context,
				const CIMObjectPath &ref,
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList &propertyList,
				InstanceResponseHandler &handler)
{
    String name(nameFromReference(ref));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	throw CIMException(CIM_ERR_NOT_FOUND);

    Header h = rpmdbNextIterator(iter);
    CIMInstance package = packageInstanceFromHeader(h);

    if (package.getPath() != ref)
	throw CIMException(CIM_ERR_NOT_FOUND);

    handler.processing();
    handler.deliver(package);
    handler.complete();
}

void
RPMProvider::getFileInstance(const OperationContext&context,
			     const CIMObjectPath&ref,
			     const Boolean includeQualifiers,
			     const Boolean includeClassOrigin,
			     const CIMPropertyList&propertyList,
			     InstanceResponseHandler &handler)
{
    String name(nameFromReference(ref).getCString());

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	throw CIMException(CIM_ERR_NOT_FOUND);

    Header h = rpmdbNextIterator(iter);

    Array<CIMInstance> files = fileInstancesFromHeader(h);

    for (Uint32 i = 0; i < files.size(); i++) {
	if (files[i].getPath() == ref) {
	    handler.processing();
	    handler.deliver(files[i]);
	    handler.complete();
	    return;
	}
    }

    throw CIMException(CIM_ERR_NOT_FOUND);
}

void
RPMProvider::getProductInstance(const OperationContext &context,
				const CIMObjectPath& ref,
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList &propertyList,
				InstanceResponseHandler &handler)
{
    String name(nameFromReference(ref));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	throw CIMException(CIM_ERR_NOT_FOUND);

    Header h = rpmdbNextIterator(iter);
    CIMInstance product = productInstanceFromHeader(h);

    if (product.getPath() != ref)
	throw CIMException(CIM_ERR_NOT_FOUND);

    handler.processing();
    handler.deliver(product);
    handler.complete();
}

//
// Instance provider methods
//

void
RPMProvider::getInstance(const OperationContext &context,
			 const CIMObjectPath &ref,
			 const Boolean includeQualifiers,
			 const Boolean includeClassOrigin,
			 const CIMPropertyList &propertyList,
			 InstanceResponseHandler &handler)
{
    if (ref.getClassName() == PackageClassName)
	return getPackageInstance(context, ref, includeQualifiers,
				  includeClassOrigin, propertyList, handler);

    if (ref.getClassName() == FileClassName)
	return getFileInstance(context, ref, includeQualifiers,
			       includeClassOrigin, propertyList, handler);

    if (ref.getClassName() == ProductClassName)
	return getProductInstance(context, ref, includeQualifiers,
				  includeClassOrigin, propertyList, handler);

    throw CIMException(CIM_ERR_NOT_FOUND);    
}

void
RPMProvider::enumerateInstances(const OperationContext &context,
			        const CIMObjectPath &ref,
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
			        const CIMPropertyList &propertyList,
			        InstanceResponseHandler &handler)
{
    handler.processing();

    if (ref.getClassName() == PackageClassName)
        handler.deliver(enumeratePackageInstances());
                                     
    if (ref.getClassName() == FileClassName)
        handler.deliver(enumerateFileInstances());

    if (ref.getClassName() == ProductClassName)
        handler.deliver(enumerateProductInstances());

    handler.complete();
}

void
RPMProvider::enumerateInstanceNames(const OperationContext &context,
				    const CIMObjectPath &ref,
				    ObjectPathResponseHandler &handler)
{
    handler.processing();

    if (ref.getClassName() == PackageClassName)
        handler.deliver(enumeratePackageInstanceNames());
                                     
    if (ref.getClassName() == FileClassName)
        handler.deliver(enumerateFileInstanceNames());

    if (ref.getClassName() == ProductClassName)
        handler.deliver(enumerateProductInstanceNames());

    handler.complete();
}

// Write operations are not supported

void
RPMProvider::modifyInstance(const OperationContext &context,
			    const CIMObjectPath &ref,
			    const CIMInstance &instanceObject,
			    const Boolean includeQualifiers,
			    const CIMPropertyList &propertyList,
			    ResponseHandler &handler)
{
    throw CIMNotSupportedException("RPMProvider "
				   "does not support modifyInstance");
}

void
RPMProvider::createInstance(const OperationContext &context,
			    const CIMObjectPath &ref,
			    const CIMInstance &instanceObject,
			    ObjectPathResponseHandler &handler)
{
    throw CIMNotSupportedException("RPMProvider "
				   "does not support createInstance");
}

void
RPMProvider::deleteInstance(const OperationContext &context,
			    const CIMObjectPath &ref,
			    ResponseHandler &handler)
{
    throw CIMNotSupportedException("RPMProvider "
				   "does not support deleteInstance");
}

//
// Association provider methods
//

// Given a PG_RPMPackage instance name, return all the PG_RPMFileCheck
// instances associated with it.

void 
RPMProvider::packageAssociators(const OperationContext &context,
				const CIMObjectPath &objectName,
				const CIMName &associationClass,
				const CIMName &resultClass,
				const String &role,
				const String &resultRole,
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
				const CIMPropertyList &propertyList,
				ObjectResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	    
	// Check for package match

	CIMInstance package = packageInstanceFromHeader(h);

	if (package.getPath() != objectName)
	    continue;

	// Return associated files

	Array<CIMInstance> files = fileInstancesFromHeader(h);
            
	for (Uint32 i = 0; i < files.size(); i++)
	    handler.deliver(files[i]);
    }

    handler.complete();
}

// Given a PG_RPMFileCheck instance name, return the PG_RPMPackage
// instance associated with it.

void 
RPMProvider::fileAssociators(const OperationContext &context,
			     const CIMObjectPath &objectName,
			     const CIMName &associationClass,
			     const CIMName &resultClass,
			     const String &role,
			     const String &resultRole,
			     const Boolean includeQualifiers,
			     const Boolean includeClassOrigin,
			     const CIMPropertyList &propertyList,
			     ObjectResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	Array<CIMObjectPath> files = fileObjectPathsFromHeader(h);

	for (Uint32 i = 0; i < files.size(); i++) {
	    if (files[i] == objectName) {
		handler.deliver(packageInstanceFromHeader(h));
		break;
	    }
	}
    }

    handler.complete();
}

// Return CIMObjectPath's for files associated with a package

void 
RPMProvider::packageAssociatorNames(const OperationContext &context,
				    const CIMObjectPath &objectName,
				    const CIMName &associationClass,
				    const CIMName &resultClass,
				    const String &role,
				    const String &resultRole,
				    ObjectPathResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	    
	// Check for package match

	CIMObjectPath package(packageObjectPathFromHeader(h));

	if (package != objectName)
	    continue;

	// Return associated files

	Array<CIMObjectPath> files(fileObjectPathsFromHeader(h));
            
	for (Uint32 i = 0; i < files.size(); i++)
	    handler.deliver(files[i]);
    }

    handler.complete();
}

// Return CIMObjectPath for package associated with a file
   
void 
RPMProvider::fileAssociatorNames(const OperationContext &context,
				 const CIMObjectPath &objectName,
				 const CIMName &associationClass,
				 const CIMName &resultClass,
				 const String &role,
				 const String &resultRole,
				 ObjectPathResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	Array<CIMInstance> files = fileInstancesFromHeader(h);

	for (Uint32 i = 0; i < files.size(); i++) {
	    if (files[i].getPath() == objectName) {
		handler.deliver(packageObjectPathFromHeader(h));
		break;
	    }
	}
    }

    handler.complete();
}

void 
RPMProvider::associators(const OperationContext &context,
			 const CIMObjectPath &objectName,
			 const CIMName &associationClass,
			 const CIMName &resultClass,
			 const String &role,
			 const String &resultRole,
			 const Boolean includeQualifiers,
			 const Boolean includeClassOrigin,
			 const CIMPropertyList &propertyList,
			 ObjectResponseHandler &handler)
{
    if (objectName.getClassName() == PackageClassName)
	return packageAssociators(context,
				  objectName,
				  associationClass,
				  resultClass,
				  role,
				  resultRole,
				  includeQualifiers,
				  includeClassOrigin,
				  propertyList,
				  handler);

    if (objectName.getClassName() == FileClassName)
	return fileAssociators(context,
			       objectName,
			       associationClass,
			       resultClass,
			       role,
			       resultRole,
			       includeQualifiers,
			       includeClassOrigin,
			       propertyList,
			       handler);
}

void 
RPMProvider::associatorNames(const OperationContext &context,
			     const CIMObjectPath &objectName,
			     const CIMName &associationClass,
			     const CIMName &resultClass,
			     const String &role,
			     const String &resultRole,
			     ObjectPathResponseHandler &handler)
{
    if (objectName.getClassName() == PackageClassName)
        return packageAssociatorNames(context,
				      objectName,
				      associationClass,
				      resultClass,
				      role,
				      resultRole,
				      handler);

    if (objectName.getClassName() == FileClassName)
        return fileAssociatorNames(context,
                                   objectName,
                                   associationClass,
                                   resultClass,
                                   role,
                                   resultRole,
                                   handler);
}

// Given a PG_RPMPackage instance name, return all the
// PG_RPMAssociatedFile instances referenced by it.

void 
RPMProvider::packageReferences(const OperationContext &context,
			       const CIMObjectPath &objectName,
			       const CIMName &resultClass,
			       const String &role,
			       const Boolean includeQualifiers,
			       const Boolean includeClassOrigin,
			       const CIMPropertyList &propertyList,
			       ObjectResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	    
	// Check for package match

	CIMObjectPath package = packageObjectPathFromHeader(h);

	if (package != objectName)
	    continue;

	// Return associated files

	Array<CIMInstance> files = fileReferencesFromHeader(h);
            
	for (Uint32 i = 0; i < files.size(); i++)
	    handler.deliver(files[i]);
    }

    handler.complete();
}

// Given a PG_RPMFileCheck instance name, return the
// PG_RPMAssociatedFile instance referenced by it.

void 
RPMProvider::fileReferences(const OperationContext &context,
			    const CIMObjectPath &objectName,
			    const CIMName &resultClass,
			    const String &role,
			    const Boolean includeQualifiers,
			    const Boolean includeClassOrigin,
			    const CIMPropertyList &propertyList,
			    ObjectResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {

	Array<CIMInstance> files = fileInstancesFromHeader(h);

	for (Uint32 i = 0; i < files.size(); i++) {
	    if (files[i].getPath() == objectName) {
		handler.deliver(packageReferenceFromHeader
				(h, files[i].getPath()));
		break;
	    }
	}
    }

    handler.complete();
}

// Given a PG_RPMPackage instance name, return all the
// PG_RPMAssociatedFile instance names referenced by it.

void
RPMProvider::packageReferenceNames(const OperationContext &context,
				   const CIMObjectPath &objectName,
				   const CIMName &resultClass,
				   const String &role,
				   ObjectPathResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {
	    
	// Check for package match

	CIMObjectPath package = packageObjectPathFromHeader(h);

	if (package != objectName)
	    continue;

	// Return associated files

	Array<CIMObjectPath> files = fileReferenceNamesFromHeader(h);
            
	for (Uint32 i = 0; i < files.size(); i++)
	    handler.deliver(files[i]);
    }

    handler.complete();
}

// Given a PG_RPMFileCheck instance name, return the
// PG_RPMAssociatedFile instance name referenced by it.

void
RPMProvider::fileReferenceNames(const OperationContext &context,
				const CIMObjectPath &objectName,
				const CIMName &resultClass,
				const String &role,
				ObjectPathResponseHandler &handler)
{
    String name(nameFromReference(objectName));

    if (name == String::EMPTY)
	return;

    IterWrapper iter(rpmdbInitIterator
		     (_db, RPMTAG_NAME, (const char *)name.getCString(), 0));

    if (iter.isNull())
	return;

    handler.processing();

    for (Header h = rpmdbNextIterator(iter); h; h = rpmdbNextIterator(iter)) {

	Array<CIMInstance> files = fileInstancesFromHeader(h);

	for (Uint32 i = 0; i < files.size(); i++) {
	    if (files[i].getPath() == objectName) {
		handler.deliver(packageReferenceNameFromHeader
				(h, files[i].getPath()));
		break;
	    }
	}
    }

    handler.complete();
}

void 
RPMProvider::references(const OperationContext &context,
			const CIMObjectPath &objectName,
			const CIMName &resultClass,
			const String &role,
			const Boolean includeQualifiers,
			const Boolean includeClassOrigin,
			const CIMPropertyList &propertyList,
			ObjectResponseHandler &handler)
{
    if (objectName.getClassName() == PackageClassName)
	return packageReferences(context,
				 objectName,
				 resultClass,
				 role,
				 includeQualifiers,
				 includeClassOrigin,
				 propertyList,
				 handler);

    if (objectName.getClassName() == FileClassName)
	return fileReferences(context,
			      objectName,
			      resultClass,
			      role,
			      includeQualifiers,
			      includeClassOrigin,
			      propertyList,
			      handler);
}

void 
RPMProvider::referenceNames(const OperationContext &context,
			    const CIMObjectPath &objectName,
			    const CIMName &resultClass,
			    const String &role,
			    ObjectPathResponseHandler &handler)
{
    if (objectName.getClassName() == PackageClassName)
	return packageReferenceNames(context,
				     objectName,
				     resultClass,
				     role,
				     handler);

    if (objectName.getClassName() == FileClassName)
	return fileReferenceNames(context,
				  objectName,
				  resultClass,
				  role,
				  handler);
}

//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdio>
#include <fstream>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/DeclContext.h>
#include "CIMRepository.h"
#include "RepositoryDeclContext.h"
#include "InstanceIndexFile.h"
#include "AssocTable.h"

#define INDENT_XML_FILES

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// _LoadObject()
//
//	Loads objects (classes, instances, and qualifiers) from disk to
//	memory objects.
//
////////////////////////////////////////////////////////////////////////////////

template<class Object>
void _LoadObject(
    const String& path,
    Object& object)
{
    // Get the real path of the file:

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
	throw CannotOpenFile(path);

    // Load file into memory:

    Array<Sint8> data;
    FileSystem::loadFileToMemory(data, realPath);
    data.append('\0');

    XmlParser parser((char*)data.getData());

    XmlReader::getObject(parser, object);
}

////////////////////////////////////////////////////////////////////////////////
//
// _SaveObject()
//
//	Saves objects (classes, instances, and qualifiers) from memory to
//	disk files.
//
////////////////////////////////////////////////////////////////////////////////

template<class Object>
void _SaveObject(const String& path, const Object& object)
{
    Array<Sint8> out;
    object.toXml(out);

    ArrayDestroyer<char> destroyer(path.allocateCString());
    PEGASUS_STD(ofstream) os(destroyer.getPointer() PEGASUS_IOS_BINARY);

    if (!os)
	throw CannotOpenFile(path);

#ifdef INDENT_XML_FILES
    out.append('\0');
    XmlWriter::indentedPrint(os, out.getData(), 2);
#else
    os.write((char*)out.getData(), out.size());
#endif
}

String _MakeAssocPath(
    const String& nameSpace,
    const String& repositoryRoot)
{
    String tmp = nameSpace;
    tmp.translate('/', '#');
    return String(Cat(repositoryRoot, "/", tmp, "/associations"));
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMRepository
//
//     The following are not implemented:
//
//         CIMRepository::setProperty()
//         CIMRepository::getProperty()
//         CIMRepository::modifyInstance()
//         CIMRepository::execQuery()
//         CIMRepository::execQuery()
//         CIMRepository::associators()
//         CIMRepository::associatorNames()
//         CIMRepository::referencess()
//         CIMRepository::referencesNames()
//         CIMRepository::invokeMethod()
//
//     Note that invokeMethod() will not never implemented since it is not
//     meaningful for a repository.
//
//     ATTN: need to combine instances of each class into a common file to
//     improve disk utilization (too many i-nodes in Unix).
//
//     ATTN: make operations on files non-case-sensitive.
//
////////////////////////////////////////////////////////////////////////////////

CIMRepository::CIMRepository(const String& repositoryRoot)
    : _repositoryRoot(repositoryRoot), _nameSpaceManager(repositoryRoot)
{
    _context = new RepositoryDeclContext(this);
}

CIMRepository::~CIMRepository()
{
    delete _context;
}

CIMClass CIMRepository::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // ATTN: localOnly, includeQualifiers, and includeClassOrigin are ignored
    // for now.

    String classFilePath;
    classFilePath = _nameSpaceManager.getClassFilePath(nameSpace, className);

    CIMClass cimClass;
    _LoadObject(classFilePath, cimClass);

    return cimClass;
}

Boolean CIMRepository::_getInstanceIndex(
    const String& nameSpace,
    const CIMReference& instanceName,
    String& className,
    Uint32& index,
    Boolean searchSuperClasses) const
{
    // -- Get all descendent classes of this class:

    className = instanceName.getClassName();

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Get all superclasses of this one:

    if (searchSuperClasses)
	_nameSpaceManager.getSuperClassNames(nameSpace, className, classNames);

    // -- Search each qualifying instance file for the instance:

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	CIMReference tmpInstanceName = instanceName;
	tmpInstanceName.setClassName(classNames[i]);

	// -- Lookup index of instance:

	String path = _getIndexFilePath(nameSpace, classNames[i]);

	if (InstanceIndexFile::lookup(path, tmpInstanceName, index))
	{
	    className = classNames[i];
	    return true;
	}
    }

    return false;
}

CIMInstance CIMRepository::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // -- Get the index for this instance:

    String className;
    Uint32 index;

    if (!_getInstanceIndex(nameSpace, instanceName, className, index))
    {
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, "getInstance()");
    }

    // -- Load the instance from the file:

    String path = _getInstanceFilePath(nameSpace, className, index);
    CIMInstance cimInstance;
    _LoadObject(path, cimInstance);
    return cimInstance;
}

void CIMRepository::deleteClass(
    const String& nameSpace,
    const String& className)
{
    // -- Disallow deletion if class has instances:

    String path = _getIndexFilePath(nameSpace, className);

    String realPath;

    if (FileSystem::existsNoCase(path, realPath))
	throw PEGASUS_CIM_EXCEPTION(CLASS_HAS_INSTANCES, className);

    // -- Delete the class (disallowed if there are subclasses):

    _nameSpaceManager.deleteClass(nameSpace, className);
}

void CIMRepository::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName)
{
    // -- Lookup index of entry from index file:

    String indexFilePath = _getIndexFilePath(
	nameSpace, instanceName.getClassName());

    Uint32 index;

    if (!InstanceIndexFile::lookup(indexFilePath, instanceName, index))
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, instanceName.toString());

    // -- Attempt to remove the instance file:

    String instanceFilePath = _getInstanceFilePath(
	nameSpace, instanceName.getClassName(), index);

    if (!FileSystem::removeFileNoCase(instanceFilePath))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "failed to remove file in CIMRepository::deleteInstance()");
    }

    // -- Remove entry from index file:

    InstanceIndexFile::remove(indexFilePath, instanceName);

    // -- Delete index file if it is now empty (zero size):

    Uint32 size;

    if (!FileSystem::getFileSizeNoCase(indexFilePath, size))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "unexpected failure in CIMRepository::deleteInstance()");
    }

    if (size == 0 && !FileSystem::removeFileNoCase(indexFilePath))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "unexpected failure in CIMRepository::deleteInstance()");
    }

    // -- Remove if from the association table (if it is really association).
    // -- We ignore the return value intentionally. If it is an association,
    // -- true is returned. Otherwise, true is returned.

    String assocFileName = _MakeAssocPath(nameSpace, _repositoryRoot);


    if (FileSystem::exists(assocFileName))
	AssocTable::deleteAssociation(assocFileName, instanceName);
}

void CIMRepository::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    // -- Resolve the class:
	CIMClass cimClass(newClass);
	
    cimClass.resolve(_context, nameSpace);

    // -- Create namespace manager entry:

    String classFilePath;

    _nameSpaceManager.createClass(nameSpace, cimClass.getClassName(),
	cimClass.getSuperClassName(), classFilePath);

    // -- Create the class file:

    _SaveObject(classFilePath, cimClass);
}

/*------------------------------------------------------------------------------

    This routine does the following:

    	1.  Creates two entries in the association file for each relationship
	    formed by this new assocation instance. A binary association
	    (one with two references) ties two instances together. Suppose
	    there are two instances: I1 and I2. Then two entries are created:

		I2 -> I1
		I1 -> I2

	    For a ternary relationship, six entries will be created. Suppose
	    there are three instances: I1, I2, and I3:

		I1 -> I2
		I1 -> I3
		I2 -> I1
		I2 -> I3
		I3 -> I1
		I3 -> I2

	    So for an N-ary relationship, there will be 2*N entries created.

	2.  Verifies that the association instance refers to real objects.
	    (note that an association reference may refer to either an instance
	    or a class). Throws an exception if one of the references does not
	    refer to a valid object.

------------------------------------------------------------------------------*/

void CIMRepository::_createAssociationEntries(
    const String& nameSpace,
    const CIMConstClass& cimClass,
    const CIMInstance& cimInstance,
    const CIMReference& instanceName)
{
    // Open input file:

    String assocFileName = _MakeAssocPath(nameSpace, _repositoryRoot);
    ofstream os;

    if (!OpenAppend(os, assocFileName))
	throw CannotOpenFile(assocFileName);

    // Get the association's instance name and class name:

    String assocInstanceName = instanceName.toString();
    String assocClassName = instanceName.getClassName();

    // For each property:

    for (Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
    {
	CIMConstProperty fromProp = cimInstance.getProperty(i);

	// If a reference property:

	if (fromProp.getType() == CIMType::REFERENCE)
	{
	    // For each property:

	    for (Uint32 j = 0, n = cimInstance.getPropertyCount(); j < n; j++)
	    {
		CIMConstProperty toProp = cimInstance.getProperty(j);

		// If a reference property and not the same property:

		if (toProp.getType() == CIMType::REFERENCE &&
		    fromProp.getName() != toProp.getName())
		{
		    CIMReference fromRef;
		    fromProp.getValue().get(fromRef);

		    CIMReference toRef;
		    toProp.getValue().get(toRef);

		    String fromObjectName = fromRef.toString();
		    String fromClassName = fromRef.getClassName();
		    String fromPropertyName = fromProp.getName();
		    String toObjectName = toRef.toString();
		    String toClassName = toRef.getClassName();
		    String toPropertyName = toProp.getName();

		    AssocTable::append(
			os,
			assocInstanceName,
			assocClassName,
			fromObjectName,
			fromClassName,
			fromPropertyName,
			toObjectName,
			toClassName,
			toPropertyName);
		}
	    }
	}
    }
}

// ATTN-A: Check to see if the objects referred to by the association
// really exist and throw an exception if they do not.

void CIMRepository::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    // -- Resolve the instance (looks up class):
	CIMInstance cimInstance(newInstance);

    CIMConstClass cimClass;
    cimInstance.resolve(_context, nameSpace, cimClass);
    CIMReference instanceName = cimInstance.getInstanceName(cimClass);

    // -- Make sure the class has keys (otherwise it will be impossible to
    // -- create the instance.

    if (!cimClass.hasKeys())
    {
	String message = "class has no keys: ";
	message += cimClass.getClassName();
	throw PEGASUS_CIM_EXCEPTION(FAILED, message);
    }

    // -- Be sure instance does not already exist:

    String className;
    Uint32 dummyIndex;

    if (_getInstanceIndex(nameSpace, instanceName, className, dummyIndex, true))
    {
	throw PEGASUS_CIM_EXCEPTION(ALREADY_EXISTS, instanceName.toString());
    }

    // -- Handle if association:

    if (cimClass.isAssociation())
    {
	_createAssociationEntries(nameSpace,
	    cimClass, cimInstance, instanceName);
    }

    // -- Get common base (of instance file and index file):

    String instanceFileBase = _nameSpaceManager.getInstanceFileBase(
	nameSpace, cimInstance.getClassName());

    // -- Make index file entry:

    String indexFilePath = _getIndexFilePath(
	nameSpace, cimInstance.getClassName());
    Uint32 index;

    if (!InstanceIndexFile::insert(indexFilePath, instanceName, index))
	throw PEGASUS_CIM_EXCEPTION(ALREADY_EXISTS, instanceName.toString());

    // -- Save instance to file:

    String instanceFilePath = _getInstanceFilePath(nameSpace,
	cimInstance.getClassName(), index);

    _SaveObject(instanceFilePath, cimInstance);
}

void CIMRepository::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    // -- Resolve the class:
	CIMClass cimClass(modifiedClass);

    cimClass.resolve(_context, nameSpace);

    // -- Check to see if it is okay to modify this class:

    String classFilePath;

    _nameSpaceManager.checkModify(nameSpace, cimClass.getClassName(),
	cimClass.getSuperClassName(), classFilePath);

    // -- Delete the old file containing the class:

    if (!FileSystem::removeFileNoCase(classFilePath))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "failed to remove file in CIMRepository::modifyClass()");
    }

    // -- Create new class file:

    _SaveObject(classFilePath, cimClass);
}

void CIMRepository::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance)
{
    // -- Resolve the instance (looks up the class):
	CIMInstance cimInstance(modifiedInstance);

    CIMConstClass cimClass;
    cimInstance.resolve(_context, nameSpace, cimClass);

    // -- Lookup index of entry from index file:

    CIMReference instanceName = cimInstance.getInstanceName(cimClass);

    String indexFilePath = _getIndexFilePath(
	nameSpace, instanceName.getClassName());

    Uint32 index;

    if (!InstanceIndexFile::lookup(indexFilePath, instanceName, index))
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, instanceName.toString());

    // -- Attempt to remove the instance file:

    String instanceFilePath = _getInstanceFilePath(
	nameSpace, instanceName.getClassName(), index);

    if (!FileSystem::removeFileNoCase(instanceFilePath))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "failed to remove file in CIMRepository::deleteInstance()");
    }

    // -- Save instance to file:

    _SaveObject(instanceFilePath, cimInstance);
}

Array<CIMClass> CIMRepository::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    Array<String> classNames;

    _nameSpaceManager.getSubClassNames(
	nameSpace, className, deepInheritance, classNames);

    Array<CIMClass> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	result.append(getClass(nameSpace, classNames[i], localOnly,
	    includeQualifiers, includeClassOrigin));
    }

    return result;
}

Array<String> CIMRepository::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    Array<String> classNames;

    _nameSpaceManager.getSubClassNames(
	nameSpace, className, deepInheritance, classNames);

    return classNames;
    return Array<String>();
}

Array<CIMInstance> CIMRepository::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // -- Get all descendent classes of this class:

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(
	nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Search each qualifying instance file for the instance:

    Array<CIMReference> instanceNames;
    Array<Uint32> indices;
    Array<CIMInstance> instances;
    Uint32 start = 0;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	// -- Form the name of the class index file:

	String path = _getIndexFilePath(nameSpace, classNames[i]);

	// Get all instance names for that class:

	InstanceIndexFile::appendInstanceNamesTo(path, instanceNames, indices);
	PEGASUS_ASSERT(instanceNames.size() == indices.size());

	// -- Load up all the instances of this class:

	for (Uint32 j = start; j < instanceNames.size(); j++)
	{
	    String instanceFilePath = _getInstanceFilePath(
		nameSpace, classNames[i], indices[i]);

	    CIMInstance tmpInstance;
	    _LoadObject(instanceFilePath, tmpInstance);
	    instances.append(tmpInstance);
	}

	start = instanceNames.size();
    }

    return instances;
}

Array<CIMReference> CIMRepository::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    // -- Get all descendent classes of this class:

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Search each qualifying instance file for the instance:

    Array<CIMReference> instanceNames;
    Array<Uint32> indices;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	// -- Form the name of the class index file:

	String path = _getIndexFilePath(nameSpace, classNames[i]);

	// Get all instances for that class:

	InstanceIndexFile::appendInstanceNamesTo(path, instanceNames, indices);
    }

    return instanceNames;
}

Array<CIMInstance> CIMRepository::execQuery(
    const String& queryLanguage,
    const String& query)
{
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "execQuery()");

    return Array<CIMInstance>();
}

Array<CIMObjectWithPath> CIMRepository::associators(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Array<CIMReference> names = associatorNames(
	nameSpace,
	objectName,
	assocClass,
	resultClass,
	role,
	resultRole);

    Array<CIMObjectWithPath> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
	String tmpNameSpace = names[i].getNameSpace();

	if (tmpNameSpace.size() == 0)
	    tmpNameSpace = nameSpace;

	CIMReference reference = names[i];

	if (reference.isClassName())
	{
	    CIMClass cimClass = getClass(
		tmpNameSpace,
		reference.getClassName(),
		false,
		includeQualifiers,
		includeClassOrigin,
		propertyList);

	    CIMObject cimObject(cimClass);
	    result.append(CIMObjectWithPath(reference, cimObject));
	}
	else
	{
	    CIMInstance cimInstance = getInstance(
		tmpNameSpace,
		reference,
		false,
		includeQualifiers,
		includeClassOrigin,
		propertyList);

	    CIMObject cimObject(cimInstance);
	    result.append(CIMObjectWithPath(reference, cimObject));
	}
    }

    return result;
}

Array<CIMReference> CIMRepository::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    String assocFileName = _MakeAssocPath(nameSpace, _repositoryRoot);
    Array<String> associatorNames;

    // The return value of this function is ignored since it is okay for
    // the given object not to have any associators (in this case we just
    // return a zero-sized array of associators.

    AssocTable::getAssociatorNames(
	assocFileName,
	objectName.toString(),
        assocClass,
        resultClass,
        role,
        resultRole,
	associatorNames);

    Array<CIMReference> result;

    for (Uint32 i = 0, n = associatorNames.size(); i < n; i++)
	result.append(associatorNames[i]);

    return result;
}

Array<CIMInstance> CIMRepository::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Array<CIMReference> names = referenceNames(
	nameSpace,
	objectName,
	resultClass,
	role);

    Array<CIMInstance> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
	String tmpNameSpace = names[i].getNameSpace();

	if (tmpNameSpace.size() == 0)
	    tmpNameSpace = nameSpace;

	CIMInstance instance = getInstance(
	    tmpNameSpace,
	    names[i],
	    false,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList);

	result.append(instance);
    }

    return result;
}

Array<CIMReference> CIMRepository::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    String assocFileName = _MakeAssocPath(nameSpace, _repositoryRoot);
    Array<String> tmpReferenceNames;

    if (!AssocTable::getReferenceNames(
	assocFileName,
	objectName,
        resultClass,
        role,
	tmpReferenceNames))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED, "references not found for: "
	    + objectName.toString());
    }

    Array<CIMReference> result;

    for (Uint32 i = 0, n = tmpReferenceNames.size(); i < n; i++)
	result.append(tmpReferenceNames[i]);

    return result;
}

CIMValue CIMRepository::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
    // -- Get the index for this instance:

    String className;
    Uint32 index;

    if (!_getInstanceIndex(nameSpace, instanceName, className, index))
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, "getProperty()");

    // -- Load the instance into memory:

    String path = _getInstanceFilePath(nameSpace, className, index);
    CIMInstance cimInstance;
    _LoadObject(path, cimInstance);

    // -- Grab the property from the instance:

    Uint32 pos = cimInstance.findProperty(propertyName);

    if (pos == PEGASUS_NOT_FOUND)
	throw PEGASUS_CIM_EXCEPTION(NO_SUCH_PROPERTY, "getProperty()");

    CIMProperty prop = cimInstance.getProperty(pos);

    // -- Return the value:

    return prop.getValue();
}

void CIMRepository::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    // -- Load the instance:

    CIMInstance instance = getInstance(
	nameSpace, instanceName, false, true);

    // -- Get the class:

    CIMClass cimClass = getClass(nameSpace, instance.getClassName(),
	false, true);

    // -- Save instance name:

    CIMReference oldRef = instance.getInstanceName(cimClass);

    // -- Modify the property (disallow if property is a key):

    Uint32 pos = instance.findProperty(propertyName);

    if (pos == PEGASUS_NOT_FOUND)
	throw PEGASUS_CIM_EXCEPTION(NO_SUCH_PROPERTY, "setProperty()");

    CIMProperty prop = instance.getProperty(pos);

    prop.setValue(newValue);

    // -- Disallow if instance name is changed by this operation (attempt
    // -- to modify a key property.

    CIMReference newRef = instance.getInstanceName(cimClass);

    if (oldRef != newRef)
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "setProperty(): attempted to modify a key property");
    }

    // -- Modify the instance:

    modifyInstance(nameSpace, instance);
}

CIMQualifierDecl CIMRepository::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
	nameSpace, qualifierName);

    // -- Load qualifier:

    CIMQualifierDecl qualifierDecl;

    try
    {
	_LoadObject(qualifierFilePath, qualifierDecl);
    }
    catch (CannotOpenFile&)
    {
	return CIMQualifierDecl();
    }

    return qualifierDecl;
}

void CIMRepository::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
	nameSpace, qualifierDecl.getName());

    // -- If qualifier alread exists, throw exception:

    if (FileSystem::existsNoCase(qualifierFilePath))
	throw PEGASUS_CIM_EXCEPTION(ALREADY_EXISTS, qualifierDecl.getName());

    // -- Save qualifier:

    _SaveObject(qualifierFilePath, qualifierDecl);
}

void CIMRepository::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
	nameSpace, qualifierName);

    // -- Delete qualifier:

    if (!FileSystem::removeFileNoCase(qualifierFilePath))
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, qualifierName);
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const String& nameSpace)
{
    String qualifiersRoot = _nameSpaceManager.getQualifiersRoot(nameSpace);

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(qualifiersRoot, qualifierNames))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED,
	    "enumerateQualifiers(): internal error");
    }

    Array<CIMQualifierDecl> qualifiers;

    for (Uint32 i = 0; i < qualifierNames.size(); i++)
    {
	CIMQualifierDecl qualifier = getQualifier(nameSpace, qualifierNames[i]);
	qualifiers.append(qualifier);
    }

    return qualifiers;
}

CIMValue CIMRepository::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters)
{
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "invokeMethod()");
    return CIMValue();
}

void CIMRepository::createNameSpace(const String& nameSpace)
{
    _nameSpaceManager.createNameSpace(nameSpace);
}

Array<String> CIMRepository::enumerateNameSpaces() const
{
    Array<String> nameSpaceNames;
    _nameSpaceManager.getNameSpaceNames(nameSpaceNames);
    return nameSpaceNames;
}

void CIMRepository::deleteNameSpace(const String& nameSpace)
{
    _nameSpaceManager.deleteNameSpace(nameSpace);
}

String CIMRepository::_getIndexFilePath(
    const String& nameSpace,
    const String& className) const
{
    String tmp = _nameSpaceManager.getInstanceFileBase(nameSpace, className);
    tmp.append(".idx");
    return tmp;
}

String CIMRepository::_getInstanceFilePath(
    const String& nameSpace,
    const String& className,
    Uint32 index) const
{
    String tmp = _nameSpaceManager.getInstanceFileBase(nameSpace, className);
    char extension[32];
    sprintf(extension, ".%d", index);
    tmp += extension;
    return tmp;
}

PEGASUS_NAMESPACE_END

//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_NameSpaceManager_h
#define Pegasus_NameSpace_Managerh

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/InheritanceTree.h>

PEGASUS_NAMESPACE_BEGIN

/** The NameSpace keeps track of memory resident status about a particular
    namespace.
*/
class PEGASUS_REPOSITORY_LINKAGE NameSpace
{
public:

    NameSpace(const String& nameSpacePath, const String& nameSpaceName);

    const String& getNameSpacePath() const { return _nameSpacePath; }

    const String& getNameSpaceName() const { return _nameSpaceName; }

    ~NameSpace();

    InheritanceTree& getInheritanceTree() { return _inheritanceTree; }

    /** Print this namespace. */
    void print(std::ostream& os) const;

private:

    InheritanceTree _inheritanceTree;
    String _nameSpacePath;
    String _nameSpaceName;
};

struct NameSpaceManagerRep;

/** The NameSpaceManager class manages a collection of NameSpace objects.
*/
class PEGASUS_REPOSITORY_LINKAGE NameSpaceManager
{
public:

    /** Constructor.
	@param repositoryRoot path to directory called "repository".
	@exception NoSuchDirectory if repositoryRoot not a valid directory.
    */
    NameSpaceManager(const String& repositoryRoot);

    /** Destructor.
    */
    ~NameSpaceManager();

    /** Determines whether the given namespace exists:
	@param nameSpaceName name of namespace.
	@return true if namespace eixsts; false otherwise.
    */
    Boolean nameSpaceExists(const String& nameSpaceName) const;

    /** Creates the given namespace.
	@param nameSpaceName name of namespace to be created.
	@exception CIMException(CIMException::ALREADY_EXISTS)
	@exception CannotCreateDirectory
    */
    void createNameSpace(const String& nameSpaceName);

    /** Deletes the given namespace.
	@param nameSpaceName name of namespace to be deleted.
	@exception CIMException(CIMException::INVALID_NAMESPACE)
	@exception NonEmptyNameSpace
	@exception FailedToRemoveDirectory
    */
    void deleteNameSpace(const String& nameSpaceName);

    /** Gets array of all namespace names.
	@param nameSpaceNames filled with names of all namespaces.
    */
    void getNameSpaceNames(Array<String>& nameSpaceNames) const;

    /** Print out the namespaces. */
    void print(std::ostream& os) const;

    /** Get path to the class file for the given class. 
	@param nameSpaceName name of the namespace.
	@param className name of class.
	@exception CIMException(CIMException::INVALID_CLASS)
	@exception CIMException(CIMException::INVALID_NAMESPACE)
    */
    String getClassFilePath(
	const String& nameSpaceName,
	const String& className) const;

private:

    String _repositoryRoot;
    NameSpaceManagerRep* _rep;
};

/** This exception is thrown if one attempts to remove a namespace that still
    contains classes, instances, or qualifier.
*/
class NonEmptyNameSpace : public Exception
{
public:

    NonEmptyNameSpace(const String& nameSpaceName) : Exception(
	"Attempt to delete a non-empty namespace: " + nameSpaceName) { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_NameSpaceManager_h */

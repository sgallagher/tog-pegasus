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
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Pair.h>
#include "InstanceIndexFile.h"
#include "NameSpaceManager.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManagerRep
//
////////////////////////////////////////////////////////////////////////////////

typedef HashTable <String, NameSpace*, EqualNoCaseFunc, HashLowerCaseFunc>
    Table;

struct NameSpaceManagerRep
{
    Table table;

    FileBasedStore* persistentStore;
};


////////////////////////////////////////////////////////////////////////////////
//
// NameSpace
//
////////////////////////////////////////////////////////////////////////////////

struct specialNameSpace;

class NameSpace
{
   friend class NameSpaceManager;
public:

    NameSpace(
        const CIMNamespaceName& nameSpaceName,
        Array<Pair<String, String> > classList,
        specialNameSpace* pns);

    void modify(
        Boolean shareable,
        Boolean updatesAllowed);

    ~NameSpace();

    static NameSpace* newNameSpace(
        int index,
        NameSpaceManager* nsm);

    Boolean readOnly() { return ro; }
    NameSpace* primaryParent();
    NameSpace* rwParent();

    CIMName getSuperClassName(const CIMName& className) const;

    const CIMNamespaceName& getNameSpaceName() const { return _nameSpaceName; }

    InheritanceTree& getInheritanceTree() { return _inheritanceTree; }

    /** Print this namespace. */
    void print(PEGASUS_STD(ostream)& os) const;

private:

    /** Build an inheritance tree based on the classes and their superclasses
        contained in the specified namespace.
    */
    void _buildInheritanceTree(
        const CIMNamespaceName& nameSpace,
        Array<Pair<String, String> > classList,
        InheritanceTree* parentTree = NULL,
        NameSpace* ns = NULL);

    InheritanceTree _inheritanceTree;
    CIMNamespaceName _nameSpaceName;

    NameSpace* parent;
    NameSpace* dependent;
    NameSpace* nextDependent;
    Boolean ro, final;
    String remoteInfo;
};

static Array<String>* nameSpaceNames = NULL;
static Array<specialNameSpace*>* specialNames = NULL;

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
struct specialNameSpace
{
    specialNameSpace()
       : shared(false),
         parentSpace(NULL),
         remote(false)
    {
    }

    void setShared(bool r, bool f, String p)
    {
        shared = true;
        ro = r;
        final = f;
        parentSpace = NULL;
        parent = p;
    }

    void setShared(bool r, bool f, NameSpace* pns, String p)
    {
        shared = true;
        ro = r;
        final = f;
        parentSpace = pns;
        parent = p;
    }

    void setRemote(String id, String host, String port, String x)
    {
        remote = true;
        remId = id;
        remHost = host;
        remPort = port;
        remInfo = x;
    }

    Boolean shared;
    Boolean ro;
    Boolean final;
    NameSpace* parentSpace;
    String parent;

    Boolean remote;
    String remId;
    String remHost;
    String remPort;
    String remInfo;
};
#else
struct specialNameSpace
{
    specialNameSpace(bool r, bool f, String p)
        : ro(r), final(f), parentSpace(NULL), parent(p) {}
    specialNameSpace(bool r, bool f, NameSpace* pns, String p)
        : ro(r), final(f), parentSpace(pns), parent(p) {}
    Boolean ro;
    Boolean final;
    NameSpace* parentSpace;
    String parent;
};
#endif

NameSpace::NameSpace(
    const CIMNamespaceName& nameSpaceName,
    Array<Pair<String, String> > classList,
    specialNameSpace* pns)
    : _nameSpaceName(nameSpaceName),
      parent(NULL),
      dependent(NULL),
      nextDependent(NULL),
      ro(false),
      final(false)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::NameSpace()");

    if (pns == NULL)
    {
        _buildInheritanceTree(nameSpaceName, classList);
    }
    else
    {
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        if (pns->shared)
        {
#endif
            ro = pns->ro;
            final = pns->final;
            parent = pns->parentSpace;
            if (parent == NULL)
            {
                _buildInheritanceTree(nameSpaceName, classList);
            }
            else
            {
                if (!ro)
                {
                    _buildInheritanceTree(
                        nameSpaceName,
                        classList,
                        &parent->_inheritanceTree,
                        this);
                }

                NameSpace* ens = parent->primaryParent();
                nextDependent = ens->dependent;
                ens->dependent = this;
            }
#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        }
        else
        {
            _buildInheritanceTree(nameSpaceName, classList);
        }

        if (pns->remote)
        {
            PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                "Remote namespace: " + nameSpaceName.getString() + " >" +
                pns->remInfo);
            remoteInfo = pns->remInfo;
        }
#endif
    }

    PEG_METHOD_EXIT();
}

NameSpace::~NameSpace()
{
}

NameSpace* NameSpace::newNameSpace(
    int index,
    NameSpaceManager* nsm)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::newNameSpace()");

    AutoPtr<NameSpace> nameSpace;

    String nameSpaceName = (*nameSpaceNames)[index];
    nameSpace.reset(nsm->_lookupNameSpace(nameSpaceName));
    if ((nameSpace.get()) != 0)
    {
        PEG_METHOD_EXIT();
        return nameSpace.release();
    }

    specialNameSpace* pns = (*specialNames)[index];

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
    if (pns && pns->shared && pns->parent.size())
#else
    if (pns && pns->parent.size())
#endif
    {
        int j = 0, m = 0;

        for (m = nameSpaceNames->size(); j < m; j++)
            if ((*nameSpaceNames)[j] == pns->parent)
                break;

        if (j >= m)
        {
            PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                "Namespace not found in parent namespace.");
        }
        pns->parentSpace=newNameSpace(j, nsm);
    }
    else if (pns)
        pns->parentSpace = NULL;

    Array<Pair<String, String> > classList =
        nsm->_rep->persistentStore->enumerateClassNames(nameSpaceName);

    nameSpace.reset(new NameSpace(nameSpaceName, classList, pns));

    nsm->_rep->table.insert(nameSpaceName, nameSpace.get());
    PEG_METHOD_EXIT();
    return nameSpace.release();
}

void NameSpace::modify(
    Boolean shareable,
    Boolean updatesAllowed)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::modify()");

    ro = !updatesAllowed;
    final = !shareable;

    PEG_METHOD_EXIT();
}

NameSpace* NameSpace::primaryParent()
{
    if (parent == NULL)
        return this;
    return parent->primaryParent();
}

NameSpace* NameSpace::rwParent()
{
   if (!ro)
       return this;
   return parent->rwParent();
}

CIMName NameSpace::getSuperClassName(const CIMName& className) const
{
    CIMName superClassName;

    if (!_inheritanceTree.getSuperClass(className, superClassName))
    {
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_FOUND, className.getString());
    }

    return superClassName;
}

void NameSpace::print(PEGASUS_STD(ostream)& os) const
{
    os << "=== NameSpace: " << _nameSpaceName.getString() << PEGASUS_STD(endl);
    _inheritanceTree.print(os);
}

void NameSpace::_buildInheritanceTree(
    const CIMNamespaceName& nameSpace,
    Array<Pair<String, String> > classList,
    InheritanceTree* parentTree,
    NameSpace* ns)
{
    for (Uint32 i = 0; i < classList.size(); i++)
    {
        if (ns)
            _inheritanceTree.insert(
                classList[i].first, classList[i].second, *parentTree, ns);
        else
            _inheritanceTree.insert(classList[i].first, classList[i].second);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManager
//
////////////////////////////////////////////////////////////////////////////////

NameSpaceManager::NameSpaceManager(
    FileBasedStore* persistentStore)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::NameSpaceManager()");

    _rep = new NameSpaceManagerRep;
    _rep->persistentStore = persistentStore;

    Array<NamespaceDefinition> nameSpaces =
        _rep->persistentStore->enumerateNameSpaces();

    nameSpaceNames = new Array<String>;
    specialNames = new Array<specialNameSpace*>;

    for (Uint32 i = 0; i < nameSpaces.size(); i++)
    {
        specialNameSpace* sns = 0;

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
        if (nameSpaces[i].shared || nameSpaces[i].remote)
        {
            sns = new specialNameSpace();
            if (nameSpaces[i].shared)
            {
                sns->setShared(
                    !nameSpaces[i].updatesAllowed,
                    !nameSpaces[i].shareable,
                    nameSpaces[i].parentNameSpace.getString());
            }
            if (nameSpaces[i].remote)
            {
                sns->setRemote(
                    nameSpaces[i].remoteId,
                    nameSpaces[i].remoteHost,
                    nameSpaces[i].remotePort,
                    nameSpaces[i].remoteInfo);
            }
        }
#else
        if (nameSpaces[i].shareable || !nameSpaces[i].updatesAllowed ||
            !nameSpaces[i].parentNameSpace.isNull())
        {
            sns = new specialNameSpace(
                !nameSpaces[i].updatesAllowed,
                !nameSpaces[i].shareable,
                nameSpaces[i].parentNameSpace.getString());
        }
#endif

        if (sns == NULL)
        {
            nameSpaceNames->prepend(nameSpaces[i].name.getString());
            specialNames->prepend(NULL);
        }
        else
        {
            nameSpaceNames->append(nameSpaces[i].name.getString());
            specialNames->append(sns);
        }
    }

    // Make sure all the parent namespaces exist

    for (int i = 0, m = nameSpaceNames->size(); i < m; i++)
    {
        specialNameSpace* pns = (*specialNames)[i];
        if (pns && pns->parent.size())
        {
            int j;
            for (j = 0; j < m; j++)
                // ATTN: Should not check self?
                if (String::equalNoCase((*nameSpaceNames)[j], pns->parent))
                    break;
            if (j >= m)
            {
                PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL2,
                    "Namespace: " + (*nameSpaceNames)[i] +
                    " ignored - parent namespace not found: " +
                    pns->parent);
                nameSpaceNames->remove(i);
                delete (*specialNames)[i];
                specialNames->remove(i);
                i = -1;   //restart
            }
        }
    }

    // Create a NameSpace object for each namespace.

    for (int i = 0, m = nameSpaceNames->size(); i < m; i++)
    {
        NameSpace::newNameSpace(i, this);
    }

    delete nameSpaceNames;
    if (specialNames)
    {
        for (int i = 0, m = specialNames->size(); i < m; i++)
            delete (*specialNames)[i];
        delete specialNames;
    }
    nameSpaceNames = NULL;
    specialNames = NULL;
    PEG_METHOD_EXIT();
}

NameSpaceManager::~NameSpaceManager()
{
    for (Table::Iterator i = _rep->table.start(); i; i++)
        delete i.value();

    delete _rep;
}

Boolean NameSpaceManager::nameSpaceExists(
    const CIMNamespaceName& nameSpaceName) const
{
    return _rep->table.contains(nameSpaceName.getString());
}

NameSpace* NameSpaceManager::_lookupNameSpace(const String& ns)
{
    NameSpace* tns;
    if (!_rep->table.lookup(ns, tns))
    {
        return NULL;
    }
    return tns;
}

NameSpace* NameSpaceManager::_getNameSpace(const CIMNamespaceName& ns) const
{
    NameSpace* tns;
    if (!_rep->table.lookup(ns.getString(), tns))
    {
        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL1, "Invalid NameSpace.");
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, ns.getString());
    }
    return tns;
}

void NameSpaceManager::createNameSpace(
    const CIMNamespaceName& nameSpaceName,
    Boolean shareable,
    Boolean updatesAllowed,
    const String& parent)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::createNameSpace");

    if (nameSpaceExists(nameSpaceName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_ALREADY_EXISTS, nameSpaceName.getString());
    }

    NameSpace* parentSpace = 0;
    if (parent.size() && !(parentSpace = _lookupNameSpace(parent)))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Parent namespace " + parent + " not found");
    }

    if (parentSpace && parentSpace->final)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Parent namespace " + parent + " not shareable");
    }

    if (updatesAllowed && parentSpace && parentSpace->parent)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "Parent namespace " + parent + " not a primary namespace");
    }

    // Create NameSpace object and register it:

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
    specialNameSpace pns;
    pns.setShared(!updatesAllowed, !shareable, parentSpace, parent);
#else
    specialNameSpace pns(
        !updatesAllowed, !shareable, parentSpace, parent);
#endif

    AutoPtr<NameSpace> nameSpace;

    nameSpace.reset(new NameSpace(
        nameSpaceName, Array<Pair<String, String> >(), &pns));

    _rep->table.insert(nameSpaceName.getString(), nameSpace.release());

    PEG_METHOD_EXIT();
}

void NameSpaceManager::modifyNameSpace(
    const CIMNamespaceName& nameSpaceName,
    Boolean shareable,
    Boolean updatesAllowed)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::modifyNameSpace()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    nameSpace->modify(shareable, updatesAllowed);

    PEG_METHOD_EXIT();
}

void NameSpaceManager::deleteNameSpace(const CIMNamespaceName& nameSpaceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::deleteNameSpace()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    // Remove and delete the namespace object:

    NameSpace **pd = NULL, *p, *d;
    for (p=nameSpace->parent; p; p=p->parent)
    {
        for (d = p->dependent, pd = &(p->dependent); d;
             pd = &(d->nextDependent), d = d->nextDependent)
        {
            if (d == nameSpace)
            {
                *pd = nameSpace->nextDependent;
                break;
            }
        }
    }

    Boolean success = _rep->table.remove(nameSpaceName.getString());
    PEGASUS_ASSERT(success);
    delete nameSpace;

    PEG_METHOD_EXIT();
}

Boolean NameSpaceManager::isRemoteNameSpace(
    const CIMNamespaceName& nameSpaceName,
    String& remoteInfo)
{
    NameSpace* nameSpace = 0;
    if (!_rep->table.lookup(nameSpaceName.getString(), nameSpace))
        return false;

    if (nameSpace->remoteInfo.size() == 0)
        return false;

    remoteInfo = nameSpace->remoteInfo;
    return true;
}

void NameSpaceManager::getNameSpaceNames(
    Array<CIMNamespaceName>& nameSpaceNames) const
{
    nameSpaceNames.clear();

    for (Table::Iterator i = _rep->table.start(); i; i++)
        nameSpaceNames.append(i.key());
}

Boolean NameSpaceManager::getNameSpaceAttributes(
    const CIMNamespaceName& nameSpaceName,
    Boolean& shareable,
    Boolean& updatesAllowed,
    String& parent)
{
    NameSpace* ns = _lookupNameSpace(nameSpaceName.getString());

    if (ns)
    {
        shareable = !ns->final;
        updatesAllowed = !ns->ro;
        if (ns->parent)
        {
            parent = ns->parent->_nameSpaceName.getString();
        }
        return true;
    }

    return false;
}

void NameSpaceManager::validateNameSpace(
    const CIMNamespaceName& nameSpaceName) const
{
    // Throws CIM_ERR_INVALID_NAMESPACE if the namespace does not exist.
    NameSpace* nameSpace = _getNameSpace(nameSpaceName);
}

Array<CIMNamespaceName> NameSpaceManager::getDependentSchemaNameSpaceNames(
    const CIMNamespaceName& nameSpaceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "NameSpaceManager::getDependentSchemaNameSpaceNames()");

    Array<CIMNamespaceName> nameSpaceNames;

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);
    nameSpaceNames.append(nameSpace->getNameSpaceName());
    nameSpace = nameSpace->dependent;
    while (nameSpace)
    {
        nameSpaceNames.append(nameSpace->getNameSpaceName());
        nameSpace = nameSpace->nextDependent;
    }

    PEG_METHOD_EXIT();
    return nameSpaceNames;
}

Boolean NameSpaceManager::hasDependentNameSpace(
    const CIMNamespaceName& nameSpaceName,
    CIMNamespaceName& dependentNameSpaceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::hasDependentNameSpace");

    Array<CIMNamespaceName> nameSpaceNames;
    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    for (Table::Iterator i = _rep->table.start(); i; i++)
    {
        if (i.value()->parent == nameSpace)
        {
            dependentNameSpaceName = i.value()->getNameSpaceName();
            PEG_METHOD_EXIT();
            return true;
        }
    }

    PEG_METHOD_EXIT();
    return false;
}

Array<CIMNamespaceName> NameSpaceManager::getSchemaNameSpaceNames(
    const CIMNamespaceName& nameSpaceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "NameSpaceManager::getSchemaNameSpaceNames()");

    Array<CIMNamespaceName> nameSpaceNames;

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    if (nameSpace->ro)
    {
        // Skip over a read-only namespace; no schema is defined here.
        nameSpace = nameSpace->rwParent();
    }

    nameSpaceNames.append(nameSpace->getNameSpaceName());

    if (nameSpace->parent)
    {
        // A R/W namespace may depend only on a primary namespace.
        nameSpaceNames.append(nameSpace->primaryParent()->getNameSpaceName());
    }

    PEG_METHOD_EXIT();
    return nameSpaceNames;
}

void NameSpaceManager::validateClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::validateClass");

    CIMName superClassName;
    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    if (nameSpace->ro)
    {
        // Skip over a read-only namespace; no schema is defined here.
        nameSpace = nameSpace->rwParent();
    }

    if (nameSpace->getInheritanceTree().getSuperClass(
            className, superClassName))
    {
        PEG_METHOD_EXIT();
        return;
    }

    if (nameSpace->parent)
    {
        // A R/W namespace may depend only on a primary namespace.
        if (nameSpace->primaryParent()->getInheritanceTree().getSuperClass(
                className, superClassName))
        {
            PEG_METHOD_EXIT();
            return;
        }
    }

    PEG_METHOD_EXIT();
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, className.getString());
}

CIMName NameSpaceManager::getSuperClassName(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    NameSpace* nameSpace = _getNameSpace(nameSpaceName);
    return nameSpace->getSuperClassName(className);
}

void NameSpaceManager::locateClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    CIMNamespaceName& actualNameSpaceName,
    CIMName& superClassName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::locateClass");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    if (nameSpace->parent != NULL)
    {
        if (!classExists(nameSpace, className, false))
        {
            actualNameSpaceName = nameSpace->parent->getNameSpaceName();
            superClassName = nameSpace->parent->getSuperClassName(className);
            PEG_METHOD_EXIT();
            return;
        }
    }

    actualNameSpaceName = nameSpace->getNameSpaceName();
    superClassName = nameSpace->getSuperClassName(className);
    PEG_METHOD_EXIT();
}

void NameSpaceManager::checkDeleteClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::checkDeleteClass");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    if (nameSpace->ro)
    {
        if (nameSpace->parent != NULL)
        {
            // Note: I think this is already checked in
            // CIMRepository::deleteClass
            classExists(nameSpace->parent, className, true);
        }

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
            "R/O Namespace " + nameSpace->getNameSpaceName().getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::checkSetOrDeleteQualifier(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& qualifierName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "NameSpaceManager::checkSetOrDeleteQualifier");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    if (nameSpace->ro)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED,
            "R/O Namespace " + nameSpace->getNameSpaceName().getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::deleteClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::deleteClass()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    // -- Remove the file from the inheritance tree:

    if (nameSpace->parent != NULL)
        nameSpace->getInheritanceTree().remove(
            className, nameSpace->parent->getInheritanceTree(), nameSpace);
    else
        nameSpace->getInheritanceTree().remove(
            className, nameSpace->getInheritanceTree(), NULL);

    PEG_METHOD_EXIT();
}

void NameSpaceManager::print(PEGASUS_STD(ostream)& os) const
{
    for (Table::Iterator i = _rep->table.start(); i; i++)
    {
        NameSpace* nameSpace = i.value();
        nameSpace->print(os);
    }

    os << PEGASUS_STD(endl);
}

void NameSpaceManager::checkCreateClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    const CIMName& superClassName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::checkCreateClass()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    if (nameSpace->readOnly())
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_ACCESS_DENIED, "R/O Namespace "+nameSpaceName.getString());
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Be certain class doesn't already exist:

    if (it.containsClass(className))
    {
        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL1,
            "Class already exists.");
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_ALREADY_EXISTS, className.getString());
    }

    if (nameSpace->parent)
    {
        // Check the parent namespace for a class with this name.  Since
        // this namespace is R/W, its parent namespace is a primary
        // namespace (has no parent of its own).

        InheritanceTree& parentIt = nameSpace->parent->getInheritanceTree();
        if (parentIt.containsClass(className))
        {
            PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL1,
                "Class already exists.");
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_ALREADY_EXISTS, className.getString());
        }
    }
    else
    {
        // Check for a collision in the direct dependent namespaces.  A
        // dependent of a dependent namespace must be read-only and therefore
        // cannot contain a class definition.

        NameSpace* ns = nameSpace->dependent;
        while (ns)
        {
            if (!ns->readOnly())
            {
                InheritanceTree& dependentIt = ns->getInheritanceTree();
                if (dependentIt.containsClass(className))
                {
                    PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL1,
                        "Class already exists.");
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_ALREADY_EXISTS, className.getString());
                }
            }
            ns = ns->nextDependent;
        }
    }

    // Verify the superclass exists.  This will have already been checked
    // if the class has been resolved, but we do not assume that dependency
    // here.

    if (!superClassName.isNull() && !it.containsClass(superClassName) &&
        !(nameSpace->parent &&
          nameSpace->parent->getInheritanceTree().containsClass(
             superClassName)))
    {
        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL1,
            "SuperClass does not exist.");
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_SUPERCLASS, superClassName.getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::createClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    const CIMName& superClassName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::createClass()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);
    InheritanceTree& it = nameSpace->getInheritanceTree();

    // Insert the entry into the inheritance tree

    if (nameSpace->parent &&
        (superClassName.isNull() || !it.containsClass(superClassName)))
    {
        it.insert(
            className.getString(),
            superClassName.getString(),
            nameSpace->parent->getInheritanceTree(),
            nameSpace);
    }
    else
    {
        it.insert(className.getString(), superClassName.getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::checkModifyClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    const CIMName& superClassName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::checkModifyClass");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Disallow changing of superclass:

    CIMName oldSuperClassName;

    if (!it.getSuperClass(className, oldSuperClassName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_FOUND, className.getString());
    }

    if (!superClassName.equal(oldSuperClassName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.NameSpaceManager.ATTEMPT_TO_CHANGE_SUPERCLASS",
                "attempt to change superclass"));
    }

    // -- Disallow modification of class with subclasses:

    Boolean hasSubClasses;
    it.hasSubClasses(className, hasSubClasses);

    if (hasSubClasses)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_CLASS_HAS_CHILDREN, className.getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::getSubClassNames(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames,
    Boolean enm) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getSubClassNames()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);
    NameSpace* dns = 0;

    if (className.getString()=="" && nameSpace->parent)
        enm=true;

    if (enm && nameSpace->parent)
    {
        dns=nameSpace->rwParent();
        nameSpace=nameSpace->primaryParent();
    }
    InheritanceTree& it = nameSpace->getInheritanceTree();

    if (!it.getSubClassNames(className, deepInheritance, subClassNames, dns))
    {
        if (nameSpace->parent)
        {
            if (enm == false)
            {
                dns=nameSpace->rwParent();
                nameSpace=nameSpace->primaryParent();
                InheritanceTree& parentIt = nameSpace->getInheritanceTree();
                if (parentIt.getSubClassNames(
                    className, deepInheritance, subClassNames, 0))
                {
                    PEG_METHOD_EXIT();
                    return;
                }
            }
        }
        else if (dns && enm)
        {
            InheritanceTree& parentIt = dns->rwParent()->getInheritanceTree();
            if (parentIt.getSubClassNames(
                    className, deepInheritance, subClassNames, 0))
            {
                PEG_METHOD_EXIT();
                return;
            }
        }
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::getSuperClassNames(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Array<CIMName>& subClassNames) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getSuperClassNames()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    nameSpace=nameSpace->rwParent();

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Get names of all superclasses:
    if (!it.getSuperClassNames(className, subClassNames))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

Boolean NameSpaceManager::classExists(
    NameSpace *nameSpace,
    const CIMName& className,
    Boolean throwExcp) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::classExists()");

    Boolean first=true;

    do
    {
        InheritanceTree& it = nameSpace->getInheritanceTree();

        if (it.containsClass(className))
        {
            if (throwExcp)
            {
                PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL1,
                    "Class already exists.");
                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_ALREADY_EXISTS, className.getString());
            }
            PEG_METHOD_EXIT();
            return true;
        }

        if (first)
        {
            nameSpace = nameSpace->dependent;
            first = false;
        }
        else
            nameSpace = nameSpace->nextDependent;
    } while (nameSpace);

    PEG_METHOD_EXIT();
    return false;
}

Boolean NameSpaceManager::classExists(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::classExists()");

    NameSpace* nameSpace = _getNameSpace(nameSpaceName);

    Boolean exists = classExists(nameSpace, className, false);

    PEG_METHOD_EXIT();
    return exists;
}

PEGASUS_NAMESPACE_END

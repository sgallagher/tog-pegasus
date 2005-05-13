//%2005////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Adrian Schuur (schuur@de.ibm.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, IBM (vijay.eli@in.ibm.com) for bug#3346
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Dir.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CommonUTF.h>
#include "InstanceIndexFile.h"
#include "NameSpaceManager.h"

PEGASUS_NAMESPACE_BEGIN

static char _CLASSES_DIR[] = "classes";
static char _INSTANCES_DIR[] = "instances";
static char _QUALIFIERS_DIR[] = "qualifiers";

static char _CLASSES_SUFFIX[] = "/classes";
static char _INSTANCES_SUFFIX[] = "/instances";
static char _QUALIFIERS_SUFFIX[] = "/qualifiers";
static char _ASSOCIATIONS_SUFFIX[] = "/associations";


////////////////////////////////////////////////////////////////////////////////
//
// _namespaceNameToDirName()
//
////////////////////////////////////////////////////////////////////////////////

static String _namespaceNameToDirName(const CIMNamespaceName& namespaceName)
{
    String dirName = namespaceName.getString();

    for (Uint32 i=0; i<dirName.size(); i++)
    {
        if (dirName[i] == '/')
        {
            dirName[i] = '#';
        }
    }
#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
    // All chars above 0x7F will be escape.
    return escapeStringEncoder(dirName);
#else
    return dirName;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// _dirNameToNamespaceName()
//
////////////////////////////////////////////////////////////////////////////////

static String _dirNameToNamespaceName(const String& dirName)
{
    String namespaceName = dirName;

    for (Uint32 i=0; i<namespaceName.size(); i++)
    {
        if (namespaceName[i] == '#')
        {
            namespaceName[i] = '/';
        }
    }
#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
    // All chars above 0x7F will be escape.
    return escapeStringDecoder(namespaceName);
#else
    return namespaceName;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// _MakeClassFilePath()
//
////////////////////////////////////////////////////////////////////////////////

static inline String _MakeClassFilePath(
    const String& nameSpacePath,
    const CIMName& className,
    const CIMName& superClassName)
{
    String returnString;
    if (!superClassName.isNull())
    {
        returnString.assign(nameSpacePath);
        returnString.append(_CLASSES_SUFFIX);
        returnString.append('/');
        returnString.append(className.getString());
        returnString.append('.');
        returnString.append(superClassName.getString());
    }
    else
    {
        returnString.assign(nameSpacePath);
        returnString.append(_CLASSES_SUFFIX);
        returnString.append('/');
        returnString.append(className.getString());
        returnString.append(".#");
    }
#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
    // All chars above 0x7F will be escape.
    return escapeStringEncoder(returnString);
#else
    return returnString;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// _MakeQualifierFilePath()
//
////////////////////////////////////////////////////////////////////////////////

static inline String _MakeQualifierFilePath(
    const String& nameSpacePath,
    const CIMName& qualifierName)
{
    String returnString(nameSpacePath);
    returnString.append(_QUALIFIERS_SUFFIX);
    returnString.append('/');
    returnString.append(qualifierName.getString());

#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
    // All chars above 0x7F will be escape.
    return escapeStringEncoder(returnString);
#else
    return returnString;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// _MakeInstanceDataFileBase()
//
////////////////////////////////////////////////////////////////////////////////

static inline String _MakeInstanceDataFileBase(
    const String& nameSpacePath,
    const CIMName& className)
{
    String returnString(nameSpacePath);
    returnString.append(_INSTANCES_SUFFIX);
    returnString.append('/');
    returnString.append(className.getString());

#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
    // All chars above 0x7F will be escape.
    return escapeStringEncoder(returnString);
#else
    return returnString;
#endif
}


////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManagerRep
//
////////////////////////////////////////////////////////////////////////////////

typedef HashTable <String, NameSpace *, EqualNoCaseFunc, HashLowerCaseFunc>
    Table;

struct NameSpaceManagerRep
{
    Table table;
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

    NameSpace(const String& nameSpacePath,
        const CIMNamespaceName& nameSpaceName, specialNameSpace *pns=NULL, String *extDir=NULL);

    void modify(Boolean shareable, Boolean updatesAllowed,const String& nameSpacePath);

    ~NameSpace();

    static NameSpace *newNameSpace(int index, NameSpaceManager *nsm, String &repositoryRoot);

    Boolean readOnly() { return ro; }
    NameSpace *primaryParent();
    NameSpace *rwParent();

    const String& getNameSpacePath() const { return _nameSpacePath; }

    const CIMNamespaceName& getNameSpaceName() const { return _nameSpaceName; }

    const String getClassFilePath(const CIMName& className) const;

    const String getQualifierFilePath(const CIMName& qualifierName) const;

    const String getInstanceDataFileBase(const CIMName& className) const;

    InheritanceTree& getInheritanceTree() { return _inheritanceTree; }

    /** Print this namespace. */
    void print(PEGASUS_STD(ostream)& os) const;

private:

    InheritanceTree _inheritanceTree;
    String _nameSpacePath;
    CIMNamespaceName _nameSpaceName;

    NameSpace *parent;
    NameSpace *dependent;
    NameSpace *nextDependent;
    Boolean ro,final;
    String sharedDirName;
    String remoteDirName;
};

static Array<String> *nameSpaceNames=NULL;
static Array<specialNameSpace*> *specialNames=NULL;

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
struct specialNameSpace {
   specialNameSpace()
      : shared(false), parentSpace(NULL), parent(String::EMPTY),
      sharedDirName(String::EMPTY), remote(false) {}
   void setShared(bool r, bool f, String p, String x) {
      shared=true;
      ro=r;
      final=f;
      parentSpace=NULL;
      parent=p;
      sharedDirName=x;
   }
   void setShared(bool r, bool f, NameSpace *pns, String p, String x) {
      shared=true;
      ro=r;
      final=f;
      parentSpace=pns;
      parent=p;
      sharedDirName=x;
   }
   void setRemote(String id, String host, String port, String x) {
      remote=true;
      remId=id;
      remHost=host;
      remPort=port;
      remDirName=x;
   }

   Boolean shared;
   Boolean ro;
   Boolean final;
   NameSpace *parentSpace;
   String parent;
   String sharedDirName;

   Boolean remote;
   String remId;
   String remHost;
   String remPort;
   String remDirName;
};
#else
struct specialNameSpace {
   specialNameSpace(bool r, bool f, String p, String x)
      : ro(r), final(f), parentSpace(NULL), parent(p), sharedDirName(x) {}
   specialNameSpace(bool r, bool f, NameSpace *pns, String p, String x)
       : ro(r), final(f), parentSpace(pns), parent(p), sharedDirName(x) {}
   Boolean ro;
   Boolean final;
   NameSpace *parentSpace;
   String parent;
   String sharedDirName;
};
#endif

#ifdef PEGASUS_ENABLE_REMOTE_CMPI

NameSpace::NameSpace(const String& nameSpacePath,
                     const CIMNamespaceName& nameSpaceName,
                     specialNameSpace *pns, String *extDir)
    : _nameSpacePath(nameSpacePath), _nameSpaceName(nameSpaceName),
      parent(NULL), dependent(NULL), nextDependent(NULL),
      ro(false), final(false)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::NameSpace()");

    if (pns==NULL) _inheritanceTree.insertFromPath(nameSpacePath +"/classes");

    else {
       if (pns->shared) {
          ro=pns->ro;
          final=pns->final;
          parent=pns->parentSpace;
          if (parent==NULL)
             _inheritanceTree.insertFromPath(nameSpacePath +"/classes");

          else {
             if (!pns->ro) _inheritanceTree.insertFromPath(nameSpacePath +"/classes",
                 &parent->_inheritanceTree,this);

             NameSpace *ens=parent->primaryParent();
             nextDependent=ens->dependent;
             ens->dependent=this;
          }
       }
       else _inheritanceTree.insertFromPath(nameSpacePath +"/classes");

       if (pns->remote) {
          PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
             "Remote namespace: " + nameSpacePath +" >"+pns->remDirName);
          remoteDirName=pns->remDirName;
       }
    }
    if (extDir) sharedDirName=*extDir;
}

#else

NameSpace::NameSpace(const String& nameSpacePath,
                     const CIMNamespaceName& nameSpaceName,
                     specialNameSpace *pns, String *extDir)
    : _nameSpacePath(nameSpacePath), _nameSpaceName(nameSpaceName),
      parent(NULL), dependent(NULL), nextDependent(NULL),
      ro(false), final(false)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::NameSpace()");

    if (pns==NULL) _inheritanceTree.insertFromPath(nameSpacePath +"/classes");

    else {
       ro=pns->ro;
       final=pns->final;
       parent=pns->parentSpace;
       if (parent==NULL)
          _inheritanceTree.insertFromPath(nameSpacePath +"/classes");

       else {
          if (!pns->ro) _inheritanceTree.insertFromPath(nameSpacePath +"/classes",
              &parent->_inheritanceTree,this);

          NameSpace *ens=parent->primaryParent();
          nextDependent=ens->dependent;
          ens->dependent=this;
       }
    }
    if (extDir) sharedDirName=*extDir;
}

#endif

NameSpace::~NameSpace()
{

}

#ifdef PEGASUS_ENABLE_REMOTE_CMPI

NameSpace *NameSpace::newNameSpace(int index, NameSpaceManager *nsm, String &repositoryRoot)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::newNameSpace()");

    AutoPtr<NameSpace> nameSpace;

    String nameSpaceName = _dirNameToNamespaceName((*nameSpaceNames)[index]);
    nameSpace.reset(nsm->lookupNameSpace(nameSpaceName));
    if ((nameSpace.get()) != 0) return nameSpace.release();

    specialNameSpace *pns=(*specialNames)[index];

    if (pns && pns->shared && pns->parent.size()) {
       int j=0,m=0;
       for (m=nameSpaceNames->size(); j<m; j++)
           if ((*nameSpaceNames)[j]==pns->parent) break;
       if (j>=m)
       {
          PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
             "Namespace not found in parent namespace.");
       }
       pns->parentSpace=newNameSpace(j,nsm,repositoryRoot);
    }
    else if (pns) pns->parentSpace=NULL;

    String nameSpacePath = repositoryRoot + "/" + (*nameSpaceNames)[index];
    nameSpace.reset(new NameSpace(nameSpacePath, nameSpaceName,pns));

    nsm->_rep->table.insert(nameSpaceName, nameSpace.get());
    return nameSpace.release();
}

#else

NameSpace *NameSpace::newNameSpace(int index, NameSpaceManager *nsm, String &repositoryRoot)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::newNameSpace()");

        AutoPtr<NameSpace> nameSpace;

        String nameSpaceName = _dirNameToNamespaceName((*nameSpaceNames)[index]);
        nameSpace.reset(nsm->lookupNameSpace(nameSpaceName));
        if ((nameSpace.get()) != 0) return nameSpace.release();

        specialNameSpace *pns=(*specialNames)[index];

        if (pns && pns->parent.size()) {
           int j=0,m=0;
           for (m=nameSpaceNames->size(); j<m; j++)
              if ((*nameSpaceNames)[j]==pns->parent) break;
           if (j>=m)
           {
              PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                 "Namespace not found in parent namespace.");
           }
           pns->parentSpace=newNameSpace(j,nsm,repositoryRoot);
        }
        else if (pns) pns->parentSpace=NULL;

        String nameSpacePath = repositoryRoot + "/" + (*nameSpaceNames)[index];
        nameSpace.reset(new NameSpace(nameSpacePath, nameSpaceName,pns));

    nsm->_rep->table.insert(nameSpaceName, nameSpace.get());
    return nameSpace.release();
}

#endif

void NameSpace::modify(Boolean shareable, Boolean updatesAllowed, const String& nameSpacePath)
{
     PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpace::modify()");

    String newDir=sharedDirName;
    if (newDir.size()==0) newDir="SWF";

    newDir[0]='S';
    newDir[1]=updatesAllowed ? 'W' : 'R';
    newDir[2]=shareable ? 'S' : 'F';

    String tmp=newDir;
    tmp.toLower();

    if (tmp=="swf") {
       String path = nameSpacePath+"/"+sharedDirName;
       FileSystem::removeFileNoCase(path);
       newDir="";
    }

    else if (sharedDirName!=newDir) {
       String path = nameSpacePath+"/"+newDir;
       if (!FileSystem::makeDirectory(path))
           throw CannotCreateDirectory(path);
       path = nameSpacePath+"/"+sharedDirName;
       if (sharedDirName.size())
          if (!FileSystem::removeDirectoryHier(path))
              throw CannotRemoveDirectory(path);
    }

    ro=!updatesAllowed;
    final=!shareable;

    sharedDirName=newDir;
}

NameSpace *NameSpace::primaryParent()
{
  if (parent==NULL) return this;
   return parent->primaryParent();
}

NameSpace *NameSpace::rwParent()
{
   if (!ro) return this;
   return parent->rwParent();
}

const String NameSpace::getClassFilePath(const CIMName& className) const
{
    CIMName superClassName;

    if (!_inheritanceTree.getSuperClass(className, superClassName))
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_NOT_FOUND, className.getString());

    return _MakeClassFilePath(_nameSpacePath, className, superClassName);
}

const String NameSpace::getQualifierFilePath(const CIMName& qualifierName) const
{
    return _MakeQualifierFilePath(_nameSpacePath, qualifierName);
}

const String NameSpace::getInstanceDataFileBase(const CIMName& className) const
{
    return _MakeInstanceDataFileBase(_nameSpacePath, className);
}

void NameSpace::print(PEGASUS_STD(ostream)& os) const
{
    os << "=== NameSpace: " << _nameSpaceName << PEGASUS_STD(endl);
    os << "_nameSpacePath: " << _nameSpacePath << PEGASUS_STD(endl);
    _inheritanceTree.print(os);
}

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManager
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _IsNameSpaceDir(const String& nameSpacePath)
{
    if (!FileSystem::isDirectory(nameSpacePath))
        return false;

    if (!FileSystem::isDirectory(nameSpacePath + _CLASSES_SUFFIX))
        return false;

    if (!FileSystem::isDirectory(nameSpacePath + _INSTANCES_SUFFIX))
        return false;

    if (!FileSystem::isDirectory(nameSpacePath + _QUALIFIERS_SUFFIX))
        return false;

    return true;
}

static String _CreateNameSpaceDirectories(const String& nameSpacePath,
         Boolean shareable, Boolean updatesAllowed, const String &parent)
{
    if (!FileSystem::makeDirectory(nameSpacePath))
        throw CannotCreateDirectory(nameSpacePath);

    String classesPath = nameSpacePath + _CLASSES_SUFFIX;
    String instancesPath = nameSpacePath + _INSTANCES_SUFFIX;
    String qualifiersPath = nameSpacePath + _QUALIFIERS_SUFFIX;

    if (!FileSystem::makeDirectory(classesPath))
        throw CannotCreateDirectory(classesPath);

    if (!FileSystem::makeDirectory(instancesPath))
        throw CannotCreateDirectory(instancesPath);

    if (!FileSystem::makeDirectory(qualifiersPath))
        throw CannotCreateDirectory(qualifiersPath);

    String path="";
    if (shareable || !updatesAllowed || parent.size()) {
       path=nameSpacePath+"/S"+(updatesAllowed ? "W" : "R")+(shareable ? "S" : "F")+parent;
       if (!FileSystem::makeDirectory(path))
          throw CannotCreateDirectory(path);
    }
    return path;
}

static Boolean _NameSpaceDirHierIsEmpty(const String& nameSpacePath)
{
    for (Dir dir(nameSpacePath); dir.more(); dir.next())
    {
        const char* name = dir.getName();

        if (strcmp(name, ".") != 0 &&
            strcmp(name, "..") != 0 &&
            System::strcasecmp(name, _CLASSES_DIR) != 0 &&
            System::strcasecmp(name, _INSTANCES_DIR) != 0 &&
            System::strcasecmp(name, _QUALIFIERS_DIR) != 0)
        {
            return true;
        }
    }

    String classesPath = nameSpacePath + _CLASSES_SUFFIX;
    String instancesPath = nameSpacePath + _INSTANCES_SUFFIX;
    String qualifiersPath = nameSpacePath + _QUALIFIERS_SUFFIX;

    return
        FileSystem::isDirectoryEmpty(classesPath) &&
        FileSystem::isDirectoryEmpty(instancesPath) &&
        FileSystem::isDirectoryEmpty(qualifiersPath);
}

NameSpaceManager::NameSpaceManager(const String& repositoryRoot)
    : _repositoryRoot(repositoryRoot)
{
     PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::NameSpaceManager()");

   // Create directory if does not already exist:

    if (!FileSystem::isDirectory(_repositoryRoot))
    {
        if (!FileSystem::makeDirectory(_repositoryRoot))
            throw CannotCreateDirectory(_repositoryRoot);

        // Create a root namespace per ...
        // Specification for CIM Operations over HTTP
        // Version 1.0
        // 2.5 Namespace Manipulation
        //
        // There are no intrinsic methods defined specifically for the
        // purpose of manipulating CIM Namespaces.  However, the
        // modelling of the a CIM Namespace using the class
        // __Namespace, together with the requirement that that
        // root Namespace MUST be supported by all CIM Servers,
        // implies that all Namespace operations can be supported.
        //

        _CreateNameSpaceDirectories(_repositoryRoot + "/root",false,true,String::EMPTY);
    }

    _rep = new NameSpaceManagerRep;

    nameSpaceNames=new Array<String>;
    specialNames=new Array<specialNameSpace*>;
    String tmp;

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
    for (Dir dir(repositoryRoot); dir.more(); dir.next()) {
        String dirName = dir.getName();
        if (dirName == ".." || dirName == ".") continue;
        String specialName=" ";

        specialNameSpace *sns=NULL;
        for (Dir dir(repositoryRoot+"/"+dirName); dir.more(); dir.next()) {
           specialName = dir.getName();
           tmp=specialName;
           tmp.toLower();
           if (specialName == ".." || specialName == ".") continue;

           switch (tmp[0]) {
           case 's': {
                 if ((tmp[1]=='w' || tmp[1]=='r') &&
                     (tmp[2]=='f' || tmp[2]=='s')) {
                    if (sns==NULL) sns=new specialNameSpace();
                    sns->setShared(tmp[1]=='r',
                       tmp[2]=='f',
                       specialName.subString(3),
                       specialName);
                 }
                 else
                 {
                    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                       "Namespace " + dirName +
                       " ignored - using incorrect parent namespace specification: " + specialName);
                 }
                 break;
              }
              case 'r': {
                 String id=tmp.subString(1,2);
                 Uint32 pos=specialName.find('@');
                 String host,port;
                 if (pos!=PEG_NOT_FOUND) {
                    host=specialName.subString(3,pos-3);
                    port=specialName.subString(pos+1);
                 }
                 else host=specialName.subString(3);
                 if (sns==NULL) sns=new specialNameSpace();
                 sns->setRemote(id,host,port,specialName);
                 PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                    "Remote namespace: " + dirName +" >"+specialName);
                 break;
              }
           }
        }
        if (sns==NULL) {
           nameSpaceNames->prepend(dirName);
           specialNames->prepend(NULL);
        }
        else {
           nameSpaceNames->append(dirName);
           specialNames->append(sns);
        }
     }

#else

    for (Dir dir(repositoryRoot); dir.more(); dir.next()) {
        String dirName = dir.getName();
        if (dirName == ".." || dirName == ".") continue;
        String specialName=" ";

        for (Dir dir(repositoryRoot+"/"+dirName); dir.more(); dir.next())
        {
           specialName = dir.getName();
           tmp=specialName;
           tmp.toLower();
           if (specialName == ".." || specialName == ".") continue;
           if (tmp[0]=='s') break;
        }

        if (tmp[0]=='s') {
           if ((tmp[1]=='w' || tmp[1]=='r') &&
               (tmp[2]=='f' || tmp[2]=='s')) {
              nameSpaceNames->append(dirName);
              specialNames->append(new specialNameSpace(tmp[1]=='r',
                 tmp[2]=='f',
                 specialName.subString(3),
                 specialName));

              continue;
           }
           PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
              "Namespace: " + dirName +
              " ignored - using incorrect parent namespace specification: " + specialName);
        }
        else {
           nameSpaceNames->prepend(dirName);
           specialNames->prepend(NULL);
        }
    }
#endif

    for (int i=0,m=nameSpaceNames->size(),j=0; i<m; i++) {
       String dirName = (*nameSpaceNames)[i];
       if (dirName.size()==0) continue;

       if (!_IsNameSpaceDir(repositoryRoot+"/"+dirName)) {
          (*nameSpaceNames)[i]=String::EMPTY;
          i=-1;   //restart
          PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
             "Namespace: " + dirName +
             " ignored - no sub directories found");
          continue;
       }

       specialNameSpace *pns=(*specialNames)[i];
       if (pns && pns->parent.size()) {
          if ((*nameSpaceNames)[i].size()) {
             if ((*nameSpaceNames)[i].size()) {
                for (j=0; j<m; j++)
                   if ((*nameSpaceNames)[j]==pns->parent) break;
                if (j>=m)
                {
                   PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                      "Namespace: " + (*nameSpaceNames)[i] +
                      " ignored - parent namespace not found: " + pns->parent);
                   (*nameSpaceNames)[i]=String::EMPTY;
                   i=-1;   //restart
                }
             }
          }
       }
    }

    // Create a NameSpace object for each directory under repositoryRoot.
    // This will throw an exception if the directory does not exist:

    for (int i=0, m=nameSpaceNames->size(); i<m; i++) {
       if ((*nameSpaceNames)[i].size()==0) continue;

       try {
           NameSpace::newNameSpace(i,this,_repositoryRoot);
       }
       catch (const Exception&) {
           throw;
       }
    }

    delete nameSpaceNames;
    if (specialNames) {
       for (int i=0,m=specialNames->size(); i<m; i++)
          delete (*specialNames)[i];
       delete specialNames;
    }
    nameSpaceNames=NULL;
    specialNames=NULL;
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
    return _rep->table.contains(nameSpaceName.getString ());
}

NameSpace *NameSpaceManager::lookupNameSpace(String &ns)
{
        NameSpace *tns;
        if (!_rep->table.lookup(ns, tns)) return NULL;
        return tns;
}

void NameSpaceManager::createNameSpace(const CIMNamespaceName& nameSpaceName,
    const NameSpaceAttributes &attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::createNameSpace()");

    // Throw exception if namespace already exists:

    String parent="";
    Boolean shareable=false;
    Boolean updatesAllowed=true;

    if (nameSpaceExists(nameSpaceName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_ALREADY_EXISTS, nameSpaceName.getString());
    }

    for (NameSpaceAttributes::Iterator i = attributes.start(); i; i++) {
       String key=i.key();
       if (String::equalNoCase(key,"shareable")) {
          if (String::equalNoCase(i.value(),"true")) shareable=true;
       }
       else if (String::equalNoCase(key,"updatesAllowed")) {
          if (String::equalNoCase(i.value(),"false")) updatesAllowed=false;
       }
       else if (String::equalNoCase(key,"parent"))
          parent=i.value();
       else {
          PEG_METHOD_EXIT();
          throw PEGASUS_CIM_EXCEPTION
              (CIM_ERR_NOT_SUPPORTED, nameSpaceName.getString()+
              " option not supported: "+key);
       }
    }

    NameSpace *parentSpace=0;
    if (parent.size() && !(parentSpace=lookupNameSpace(parent))) {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_NOT_FOUND, " parent namespace "+parent+" not found");
    }

    if (parentSpace && parentSpace->final) {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_FAILED, " parent namespace "+parent+" not shareable");
    }

    if (updatesAllowed && parentSpace && parentSpace->parent) {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_FAILED, " parent namespace "+parent+" not a primary namespace");
    }

#ifndef PEGASUS_SUPPORT_UTF8_FILENAME
    // Do not allow file names to contain characters outsie of 7-bit ascii.
    String tmp = nameSpaceName.getString();
    Uint32 len = tmp.size();
    for(Uint32 i = 0; i < len; ++i)
        if((Uint16)tmp[i] > 0x007F)
            throw PEGASUS_CIM_EXCEPTION
                (CIM_ERR_INVALID_PARAMETER, nameSpaceName.getString());
#endif


    // Attempt to create all the namespace diretories:

    String nameSpaceDirName = _namespaceNameToDirName(nameSpaceName);
    String nameSpacePath = _repositoryRoot + "/" + nameSpaceDirName;
    String parentPath;
    if (parent.size()) parentPath = _namespaceNameToDirName(parent);

    String extDir =
       _CreateNameSpaceDirectories(nameSpacePath,shareable,updatesAllowed,parentPath);

#ifdef PEGASUS_ENABLE_REMOTE_CMPI
    specialNameSpace pns;
    pns.setShared(!updatesAllowed,!shareable,parentSpace,parent,extDir);
#else
    specialNameSpace pns(!updatesAllowed,!shareable,parentSpace,parent,extDir);
#endif

    // Create NameSpace object and register it:

    AutoPtr<NameSpace> nameSpace;

    nameSpace.reset(new NameSpace(nameSpacePath, nameSpaceName, &pns));

    _rep->table.insert(nameSpaceName.getString (), nameSpace.release());

    PEG_METHOD_EXIT();
}

void NameSpaceManager::modifyNameSpace(const CIMNamespaceName& nameSpaceName,
        const NameSpaceAttributes &attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::modifyNameSpace()");

    NameSpace* nameSpace;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    Boolean shareable=false;
    Boolean updatesAllowed=true;

    for (NameSpaceAttributes::Iterator i = attributes.start(); i; i++) {
       String key=i.key();
       if (String::equalNoCase(key,"shareable")) {
          if (String::equalNoCase(i.value(),"true")) shareable=true;
       }
       else if (String::equalNoCase(key,"updatesAllowed")) {
          if (String::equalNoCase(i.value(),"false")) updatesAllowed=false;
       }
       else {
          PEG_METHOD_EXIT();
          throw PEGASUS_CIM_EXCEPTION
              (CIM_ERR_NOT_SUPPORTED, nameSpaceName.getString()+
              " option not supported: "+key);
       }
    }

    if (!shareable && !nameSpace->final)
          for (Table::Iterator i = _rep->table.start(); i; i++)
       if (i.value()->parent==nameSpace) {
          PEG_METHOD_EXIT();
          throw PEGASUS_CIM_EXCEPTION
             (CIM_ERR_FAILED, "namespace "+nameSpaceName.getString()+
             " has dependent namespace "+i.value()->_nameSpaceName.getString());
       }

    String nameSpaceDirName = _namespaceNameToDirName(nameSpaceName);
    String nameSpacePath = _repositoryRoot + "/" + nameSpaceDirName;

    nameSpace->modify(shareable,updatesAllowed,nameSpacePath);

    PEG_METHOD_EXIT();
}


void NameSpaceManager::deleteNameSpace(const CIMNamespaceName& nameSpaceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::deleteNameSpace()");

    // If no such namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    for (Table::Iterator i = _rep->table.start(); i; i++)
       if (i.value()->parent==nameSpace) {
          PEG_METHOD_EXIT();
          throw PEGASUS_CIM_EXCEPTION
             (CIM_ERR_FAILED, "namespace "+nameSpaceName.getString()+
             " has dependent namespace "+i.value()->_nameSpaceName.getString());
       }

    // Form namespace path:

    String nameSpaceDirName = _namespaceNameToDirName
        (nameSpace->getNameSpaceName());
    String nameSpacePath = _repositoryRoot + "/" + nameSpaceDirName;

    // Delete the entire namespace directory hierarchy:

    if (!_NameSpaceDirHierIsEmpty(nameSpacePath))
    {
        PEG_METHOD_EXIT();
        throw NonEmptyNameSpace(nameSpaceName.getString());
    }

    if (!FileSystem::removeDirectoryHier(nameSpacePath))
    {
        PEG_METHOD_EXIT();
        throw CannotRemoveDirectory(nameSpacePath);
    }

    // Remove and delete the namespace object:

    NameSpace **pd=NULL,*p,*d;
    for (p=nameSpace->parent; p; p=p->parent) {
       for (d=p->dependent,pd=&(p->dependent); d;
                 pd=&(d->nextDependent),d=d->nextDependent) {
          if (d==nameSpace) {
             *pd=nameSpace->nextDependent;
             break;
          }
       }
    }

    Boolean success = _rep->table.remove(nameSpaceName.getString ());
    PEGASUS_ASSERT(success);
    delete nameSpace;

    PEG_METHOD_EXIT();
}

Boolean NameSpaceManager::isRemoteNameSpace(const CIMNamespaceName& nameSpaceName,
        String & remoteInfo)
{
    NameSpace* nameSpace = 0;
    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace)) return false;

    if (nameSpace->remoteDirName.size()==0) return false;

    remoteInfo=nameSpace->remoteDirName;
    return true;
}

void NameSpaceManager::getNameSpaceNames(Array<CIMNamespaceName>& nameSpaceNames) const
{
    nameSpaceNames.clear();

    for (Table::Iterator i = _rep->table.start(); i; i++)
        nameSpaceNames.append(i.key());
}

Boolean NameSpaceManager::getNameSpaceAttributes(const CIMNamespaceName& nameSpaceName,
                 NameSpaceAttributes& attributes)
{
    String nsn=nameSpaceName.getString();
    NameSpace *ns=lookupNameSpace(nsn);

    if (ns) {
       if (ns->parent) attributes.insert("parent",ns->parent->_nameSpaceName.getString());
       attributes.insert("name",nsn);
       if (ns->ro) attributes.insert("updatesAllowed","false");
       else attributes.insert("updatesAllowed","true");
       if (ns->final) attributes.insert("shareable","false");
       else attributes.insert("shareable","true");
       return true;
    }
    return false;
}

String NameSpaceManager::getClassFilePath(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    NameSpaceIntendedOp op) const
{
    NameSpace *nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace)) {
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    return getClassFilePath(nameSpace,className,op);
}

String NameSpaceManager::getClassFilePath(
    NameSpace* nameSpace,
    const CIMName& className,
    NameSpaceIntendedOp op) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getClassFilePath()");

    if (nameSpace->parent==NULL) {
       if (nameSpace->ro) switch (op) {
       case NameSpaceRead:
           break;
       case NameSpaceDelete:
       case NameSpaceWrite:
           PEG_METHOD_EXIT();
           throw PEGASUS_CIM_EXCEPTION
               (CIM_ERR_ACCESS_DENIED, "R/O Namespace "+
               nameSpace->getNameSpaceName().getString());
       }
       else switch (op) {
       case NameSpaceRead:
          break;
       case NameSpaceWrite:
          classExists(nameSpace,className,true);
          break;
       case NameSpaceDelete:
          classHasInstances(nameSpace,className,true);
       }
       PEG_METHOD_EXIT();
       return nameSpace->getClassFilePath(className);
    }

    if (nameSpace->ro==false) {
       switch (op) {
       case NameSpaceRead:
          if (classExists(nameSpace,className,false)) break;
          PEG_METHOD_EXIT();
          return nameSpace->parent->getClassFilePath(className);
       case NameSpaceWrite:
          classExists(nameSpace->parent,className,true);
          break;
       case NameSpaceDelete:
          classHasInstances(nameSpace,className,true);
       }
        PEG_METHOD_EXIT();
       return nameSpace->getClassFilePath(className);
    }

    switch (op) {
    case NameSpaceRead:
       if (classExists(nameSpace,className,false)) break;
       return nameSpace->parent->getClassFilePath(className);
    case NameSpaceDelete:
       classExists(nameSpace->parent,className,true);
    case NameSpaceWrite:
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_ACCESS_DENIED, "R/O Namespace "+
            nameSpace->getNameSpaceName().getString());
    }
    PEG_METHOD_EXIT();
    return nameSpace->getClassFilePath(className);
}

String NameSpaceManager::getInstanceDataFileBase(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getInstanceDataFileBase()");

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    PEG_METHOD_EXIT();
    return nameSpace->getInstanceDataFileBase(className);
}

String NameSpaceManager::getInstanceDataFileBase(
    const NameSpace *nameSpace,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getInstanceDataFileBase()");

    PEG_METHOD_EXIT();
    return nameSpace->getInstanceDataFileBase(className);
}

String NameSpaceManager::getQualifierFilePath(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& qualifierName,
    NameSpaceIntendedOp op) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getQualifierFilePath()");

    NameSpace *nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace)) {
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    String filePath=nameSpace->getQualifierFilePath(qualifierName);

    if (nameSpace->parent==NULL) {
       if (nameSpace->ro) switch (op) {
       case NameSpaceRead:
           break;
       case NameSpaceDelete:
       case NameSpaceWrite:
           PEG_METHOD_EXIT();
           throw PEGASUS_CIM_EXCEPTION
               (CIM_ERR_ACCESS_DENIED, "R/O Namespace "+
               nameSpace->getNameSpaceName().getString());
       }
       PEG_METHOD_EXIT();
       return filePath;
    }

    if (nameSpace->ro==false) {
       switch (op) {
       case NameSpaceRead:
          if (FileSystem::existsNoCase(filePath)) break;
          PEG_METHOD_EXIT();
          return nameSpace->parent->getQualifierFilePath(qualifierName);
       case NameSpaceWrite:
       case NameSpaceDelete:
          break;
       }
       PEG_METHOD_EXIT();
       return filePath;
    }

    switch (op) {
    case NameSpaceRead:
       if (FileSystem::existsNoCase(filePath)) break;
    PEG_METHOD_EXIT();
       return nameSpace->parent->getQualifierFilePath(qualifierName);
    case NameSpaceDelete:
    case NameSpaceWrite:
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_ACCESS_DENIED, "R/O Namespace "+
            nameSpace->getNameSpaceName().getString());
    }
    PEG_METHOD_EXIT();
    return filePath;

}

void NameSpaceManager::deleteClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::deleteClass()");

    // -- Lookup NameSpace object:

    NameSpace *nameSpace=0,*ns=0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }
    // -- Get path to class file:

    String classFilePath = getClassFilePath(nameSpace,className,NameSpaceDelete);

    // -- Remove the file from the inheritance tree:

    if (nameSpace->parent!=NULL)
       nameSpace->getInheritanceTree().remove(className,
          nameSpace->parent->getInheritanceTree(),nameSpace);
    else nameSpace->getInheritanceTree().remove(className,
       nameSpace->getInheritanceTree(),NULL);

    // -- Remove the file from disk:

    if (!FileSystem::removeFileNoCase(classFilePath))
    {
        PEG_METHOD_EXIT();
        throw CannotRemoveFile(classFilePath);
    }

    Boolean first=true;
    do {
       String indexFilePath = getInstanceDataFileBase(nameSpace, className)+".idx";
       PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                        "instance indexFilePath = " + indexFilePath);

       String dataFilePath = getInstanceDataFileBase(nameSpace, className)+".instances";
       PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                        "instance dataFilePath = " + dataFilePath);

       FileSystem::removeFileNoCase(indexFilePath);
       FileSystem::removeFileNoCase(dataFilePath);

       if (first) {
          nameSpace=nameSpace->dependent;
          first=false;
       }
       else nameSpace=nameSpace->nextDependent;
    } while (nameSpace);

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

void NameSpaceManager::createClass(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    const CIMName& superClassName,
    String& classFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::createClass()");

    // -- Lookup namespace:

    NameSpace *nameSpace=0,*ens=0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Invalid NameSpace.");
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    if (nameSpace->readOnly()) {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_ACCESS_DENIED, "R/O Namespace "+nameSpaceName.getString());
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Be certain class doesn't already exist:

    if (it.containsClass(className)) {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Class already exists.");
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_ALREADY_EXISTS, className.getString());
    }

    if (nameSpace->parent) {
       InheritanceTree& it = nameSpace->parent->getInheritanceTree();
       if (it.containsClass(className)) {
           PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Class already exists.");
           PEG_METHOD_EXIT();
           throw PEGASUS_CIM_EXCEPTION
               (CIM_ERR_ALREADY_EXISTS, className.getString());
       }
    }

    else {
       NameSpace *ns=nameSpace->dependent;
       while (ns) {
          if (!ns->readOnly()) {
             InheritanceTree& it = ns->getInheritanceTree();
             if (it.containsClass(className)) {
                 PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Class already exists.");
                 PEG_METHOD_EXIT();
                 throw PEGASUS_CIM_EXCEPTION
                     (CIM_ERR_ALREADY_EXISTS, className.getString());
             }
          }
          ns=ns->nextDependent;
       }
    }

    // -- Be certain superclass exists:

    Boolean xNameSpace=false;
    Boolean missing=false;

    if (superClassName.isNull()) {
       if (nameSpace->parent) xNameSpace=true;
    }
    else if (!it.containsClass(superClassName)) {
       if (nameSpace->parent) {
          InheritanceTree& it = nameSpace->parent->getInheritanceTree();
          if (!it.containsClass(superClassName)) missing=true;
          xNameSpace=true;
       }
       else missing=false;
    }

    if (missing) {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
                         "SuperClass does not exist.");
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_SUPERCLASS, superClassName.getString());
    }

#ifndef PEGASUS_SUPPORT_UTF8_FILENAME
    // Do not allow file names to contain characters outsie of 7-bit ascii.
    String tmp = className.getString();
    Uint32 len = tmp.size();
    for(Uint32 i = 0; i < len; ++i)
        if((Uint16)tmp[i] > 0x007F)
            throw PEGASUS_CIM_EXCEPTION
                (CIM_ERR_INVALID_PARAMETER, nameSpaceName.getString());
#endif


    // -- Insert the entry:

    if (xNameSpace) it.insert(className.getString(), superClassName.getString(),
       nameSpace->parent->getInheritanceTree(),nameSpace);
    else it.insert(className.getString(), superClassName.getString());

    // -- Build the path to the class:

    classFilePath = _MakeClassFilePath(
        nameSpace->getNameSpacePath(), className, superClassName);

    PEG_METHOD_EXIT();
}

void NameSpaceManager::checkModify(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    const CIMName& superClassName,
    String& classFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::checkModify()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Disallow changing of superclass:

    CIMName oldSuperClassName;

    if (!it.getSuperClass(className, oldSuperClassName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_NOT_FOUND, className.getString());
    }

    if (!superClassName.equal (oldSuperClassName))
    {
        PEG_METHOD_EXIT();

        // l10n
        // throw PEGASUS_CIM_EXCEPTION(
        // CIM_ERR_FAILED, "attempt to change superclass");

        throw PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_FAILED,
            MessageLoaderParms("Repository.NameSpaceManager.ATTEMPT_TO_CHANGE_SUPERCLASS",
            "attempt to change superclass"));

    }

    // -- Disallow modification of class with subclasses:

    Boolean hasSubClasses;
    it.hasSubClasses(className, hasSubClasses);

    if (hasSubClasses)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_CLASS_HAS_CHILDREN, className.getString());
    }

    // -- Build the path to the class:

    classFilePath = _MakeClassFilePath(
        nameSpace->getNameSpacePath(), className, superClassName);

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

    // -- Lookup namespace:

    NameSpace *nameSpace=0,*dns=0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    if (className.getString()=="" && nameSpace->parent) enm=true;

    if (enm && nameSpace->parent) {
       dns=nameSpace->rwParent();
       nameSpace=nameSpace->primaryParent();
    }
    InheritanceTree& it = nameSpace->getInheritanceTree();

    if (!it.getSubClassNames(className, deepInheritance, subClassNames, dns)) {
        if (nameSpace->parent) {
           if (enm==false) {
              dns=nameSpace->rwParent();
              nameSpace=nameSpace->primaryParent();
              InheritanceTree& it = nameSpace->getInheritanceTree();
              if (it.getSubClassNames(className, deepInheritance, subClassNames, 0)) return;
           }
        }
        else if (dns && enm) {
           InheritanceTree& it = dns->rwParent()->getInheritanceTree();
           if (it.getSubClassNames(className, deepInheritance, subClassNames, 0)) return;
        }
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::getSuperClassNames(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Array<CIMName>& subClassNames) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getSuperClassNames()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }
    nameSpace=nameSpace->rwParent();

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Get names of all superclasses:
    if (!it.getSuperClassNames(className, subClassNames))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

String NameSpaceManager::getQualifiersRoot(const CIMNamespaceName& nameSpaceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getQualifiersRoot()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    PEG_METHOD_EXIT();
    return nameSpace->getNameSpacePath() + _QUALIFIERS_SUFFIX;
}

Array<String> NameSpaceManager::getAssocClassPath(
        const CIMNamespaceName& nameSpaceName,
        NameSpaceIntendedOp op) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getAssocClassPath()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    Array<String> assocClassPathes;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    if (nameSpace->ro) nameSpace = nameSpace->rwParent();

    assocClassPathes.append(nameSpace->getNameSpacePath() +
        _CLASSES_SUFFIX + _ASSOCIATIONS_SUFFIX);

    if (op==NameSpaceRead) {
       if (nameSpace->parent) {
          nameSpace = nameSpace->primaryParent();
          assocClassPathes.append(nameSpace->getNameSpacePath() +
             _CLASSES_SUFFIX + _ASSOCIATIONS_SUFFIX);
       }
    }

    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
        String("Association class path = ") + assocClassPathes[0]);

    PEG_METHOD_EXIT();
    return assocClassPathes;
}

String NameSpaceManager::getAssocInstPath(
    const CIMNamespaceName& nameSpaceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getAssocInstPath()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    String assocInstPath = nameSpace->getNameSpacePath() +
        _INSTANCES_SUFFIX + _ASSOCIATIONS_SUFFIX;
    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
        String("Association instance path = ") + assocInstPath);

    PEG_METHOD_EXIT();
    return assocInstPath;
}

Boolean NameSpaceManager::classHasInstances(
    NameSpace *nameSpace,
    const CIMName& className,
    Boolean throwExcp) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::classHasChildren()");

    Boolean first=true;

    do {
       String indexFilePath = getInstanceDataFileBase(nameSpace, className)+".idx";

       if (InstanceIndexFile::hasNonFreeEntries(indexFilePath)) {
           PEG_METHOD_EXIT();
           if (throwExcp) throw PEGASUS_CIM_EXCEPTION(CIM_ERR_CLASS_HAS_INSTANCES,
               className.getString());
          return true;
       }

       if (first) {
          nameSpace=nameSpace->dependent;
          first=false;
       }
       else nameSpace=nameSpace->nextDependent;

    } while (nameSpace);

    PEG_METHOD_EXIT();
    return false;
}

Boolean NameSpaceManager::classHasInstances(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Boolean throwExcp) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::classHasChildren()");

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName.getString (), nameSpace)) {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_INVALID_NAMESPACE, nameSpaceName.getString());
    }

    PEG_METHOD_EXIT();
    return classHasInstances(nameSpace,className,throwExcp);
}

Boolean NameSpaceManager::classExists(
    NameSpace *nameSpace,
    const CIMName& className,
    Boolean throwExcp) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::classHasChildren()");

    Boolean first=true;

    do {

       InheritanceTree& it = nameSpace->getInheritanceTree();

       if (it.containsClass(className)) {
           PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Class already exists.");
           PEG_METHOD_EXIT();
           if (throwExcp) throw PEGASUS_CIM_EXCEPTION
               (CIM_ERR_ALREADY_EXISTS, className.getString());
           return true;
       }

       if (first) {
          nameSpace=nameSpace->dependent;
          first=false;
       }
       else nameSpace=nameSpace->nextDependent;

    } while (nameSpace);

    PEG_METHOD_EXIT();
    return false;
}

PEGASUS_NAMESPACE_END

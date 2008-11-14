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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMNameCast.h>
#include "AssocClassTable.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static inline Boolean _MatchNoCase(const String& x, const String& pattern)
{
    return pattern.size() == 0 || String::equalNoCase(x, pattern);
}

static Boolean _GetRecord(ifstream& is, ClassAssociation& record)
{
    String line;

    if (!GetLine(is, line))
        return false;
    record.assocClassName = CIMNameCast(line);

    if (!GetLine(is, line))
        return false;
    record.fromClassName = CIMNameCast(line);

    if (!GetLine(is, line))
        return false;
    record.fromPropertyName = CIMNameCast(line);

    if (!GetLine(is, line))
        return false;
    record.toClassName = CIMNameCast(line);

    if (!GetLine(is, line))
        return false;
    record.toPropertyName = CIMNameCast(line);

    // Skip the blank line:

    if (!GetLine(is, line))
        return false;

    return true;
}

static inline void _PutField(ofstream& os, const CIMName& field)
{
    // Write the field in UTF-8.  Call write() to ensure no data
    // conversion by the stream.  Since all the fields contain CIM names,
    // it is not necessary to escape CR/LF characters.
    CString buffer = field.getString().getCString();
    os.write((const char *)buffer,
        static_cast<streamsize>(strlen((const char *)buffer)));
    os << endl;
}

static void _PutRecord(ofstream& os, const ClassAssociation& record)
{
    _PutField(os, record.assocClassName);
    _PutField(os, record.fromClassName);
    _PutField(os, record.fromPropertyName);
    _PutField(os, record.toClassName);
    _PutField(os, record.toPropertyName);
    os << endl;
}

void AssocClassTable::append(
    PEGASUS_STD(ofstream)& os,
    const String& path,
    const ClassAssociation& classAssociation)
{
    _PutRecord(os, classAssociation);

    // Update cache
    AssocClassCache* cache = _assocClassCacheManager.getAssocClassCache(path);
    if (cache->isActive())
    {
        cache->addRecord(classAssociation.fromClassName, classAssociation);
    }
}

void AssocClassTable::append(
    const String& path,
    const ClassAssociation& classAssociation)
{
    // Open input file:

    ofstream os;

    if (!OpenAppend(os, path))
        throw CannotOpenFile(path);

    _PutRecord(os, classAssociation);

    // Update cache
    AssocClassCache* cache = _assocClassCacheManager.getAssocClassCache(path);
    if (cache->isActive())
    {
        cache->addRecord(classAssociation.fromClassName, classAssociation);
    }
}

Boolean AssocClassTable::deleteAssociation(
    const String& path,
    const CIMName& assocClassName)
{
    // Open input file:

    ifstream is;
    if (!FileSystem::exists(path))
    {
        return false;
    }

    if (!Open(is, path))
    {
        throw CannotOpenFile(path);
    }

    // Open output file:

    String tmpPath = path + ".tmp";
    ofstream os;
    if (!Open(os, tmpPath))
    {
        throw CannotOpenFile(tmpPath);
    }

    // Copy over all lines except ones with the given association instance name:

    ClassAssociation classAssociation;
    ClassAssociation classAssociationToDelete;
    Boolean found = false;

    while (_GetRecord(is, classAssociation))
    {
        if (assocClassName.getString() != classAssociation.assocClassName)
        {
            _PutRecord(os, classAssociation);
            found = true;
        }
        else
        {
            classAssociationToDelete = classAssociation;
        }
    }

    // Close both files:

    is.close();
    os.close();

    // Rename back to original name:

    if (!FileSystem::renameFile(tmpPath, path))
        throw CannotRenameFile(path);


    // Update cache
    if (found)
    {
        AssocClassCache* cache =
        _assocClassCacheManager.getAssocClassCache(path);
        if (cache->isActive())
        {
            cache->removeRecord(classAssociationToDelete.fromClassName,
                                classAssociationToDelete.assocClassName);
        }
    }

    return found;
}

Boolean AssocClassTable::getAssociatorNames(
    const String& path,
    const Array<CIMName>& classList,
    const Array<CIMName>& assocClassList,
    const Array<CIMName>& resultClassList,
    const String& role,
    const String& resultRole,
    Array<String>& associatorNames)
{
    // Open input file:
    ifstream is;
    if (!FileSystem::exists(path))
    {
        return false;
    }

    if (!Open(is, path))
    {
        throw CannotOpenFile(path);
    }

    ClassAssociation classAssociation;
    Boolean found = false;

    // For each line in the associations table:
    while (_GetRecord(is, classAssociation))
    {
        // Process associations from the right end class and with right roles
        if (Contains(classList, classAssociation.fromClassName) &&
            _MatchNoCase(classAssociation.fromPropertyName.getString(), role) &&
            _MatchNoCase(
                classAssociation.toPropertyName.getString(), resultRole))
        {
            // Skip classes that do not appear in the association class list
            if ((assocClassList.size() != 0) &&
                (!Contains(assocClassList, classAssociation.assocClassName)))
            {
                continue;
            }

            // Skip classes that do not appear in the result class list
            if ((resultClassList.size() != 0) &&
                (!Contains(resultClassList, classAssociation.toClassName)))
            {
                continue;
            }

            // This class qualifies; add it to the list (skipping duplicates)
            if (!Contains(
                    associatorNames, classAssociation.toClassName.getString()))
            {
                associatorNames.append(
                    classAssociation.toClassName.getString());
            }
            found = true;
        }
    }

    return found;
}

Boolean AssocClassTable::_InitializeCache(
    AssocClassCache* cache,
    const String& path)
{
    if (!cache->isActive())
    {

        // Open input file:
        ifstream is;
        if (!FileSystem::exists(path))
        {
            return false;
        }

        if (!Open(is, path))
        {
            throw CannotOpenFile(path);
        }

        ClassAssociation classAssociation;

        // For each line in the associations table:
        while (_GetRecord(is, classAssociation))
        {
            cache->addRecord(classAssociation.fromClassName,
                             classAssociation);
        }

        cache->setActive(true);
    }

    return true;
}

Boolean AssocClassTable::getReferenceNames(
    const String& path,
    const Array<CIMName>& classList,
    const Array<CIMName>& resultClassList,
    const String& role,
    Array<String>& referenceNames)
{

    // First see if we can get the information from the association class cache.
    AssocClassCache* cache = _assocClassCacheManager.getAssocClassCache(path);

    if (!cache->isActive())
    {
        if (!_InitializeCache(cache,path))
            return false;
    }

    Array<ClassAssociation> records;
    Boolean found = false;


    // For each of the target classes retrieve the list of matching
    // association classes from the cache.
    // The cache uses the from class name as an index and returns all
    // association class records having that from class.

    for (Uint16 idx=0; idx < classList.size(); idx++)
    {
        String fromClassName = classList[idx].getString();
        if (cache->getAssocClassEntry(fromClassName, records))
        {
            for (Uint16 rx=0; rx <records.size(); rx++)
            {
                if (_MatchNoCase(
                        records[rx].fromPropertyName.getString(), role))
                {
                    // Skip classes that do not appear in the result class list
                    if ((resultClassList.size() != 0) &&
                        (!Contains(resultClassList,
                             records[rx].assocClassName)))
                    {
                        continue;
                    }

                    // This class qualifies; add it to the list (skipping
                    // duplicates)
                    if (!Contains(referenceNames,
                            records[rx].assocClassName.getString()))
                    {
                        referenceNames.append(
                            records[rx].assocClassName.getString());
                    }
                    found = true;
                }
            }
        }
    }

    return found;
}

PEGASUS_NAMESPACE_END

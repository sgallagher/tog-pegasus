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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Robert Kieninger, IBM (kieningr@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include "AssocClassTable.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define ASSOC_CLASS_NAME_INDEX 0
#define FROM_CLASS_NAME_INDEX 1
#define FROM_PROPERTY_NAME_INDEX 2
#define TO_CLASS_NAME_INDEX 3
#define TO_PROPERTY_NAME_INDEX 4
#define NUM_FIELDS 5


ReadWriteSem AssocClassTable::_classCacheLock;


static inline Boolean _MatchNoCase(const String& x, const String& pattern)
{
    return pattern.size() == 0 || String::equalNoCase(x, pattern);
}

static inline Boolean _ContainsClass(const Array<CIMName>& classNames, const String& match )
{
    Uint32 n = classNames.size();

    for (Uint32 i = 0; i < n; i++)
    {
        if (_MatchNoCase(classNames[i].getString(), match))
            return true;
    }

    return false;
}


static String _Escape(const String& str)
{
    String result;

    for (Uint32 i = 0, n = str.size(); i < n; i++)
    {
	Char16 c = str[i];

	switch (c)
	{
	    case '\n':
		result.append("\\n");
		break;

	    case '\r':
		result.append("\\r");
		break;

	    case '\t':
		result.append("\\t");
		break;

	    case '\f':
		result.append("\\f");
		break;

	    case '\\':
		result.append("\\\\");
		break;

	    default:
		result.append(c);
	}
    }

    return result;
}

static String _Unescape(const String& str)
{
    String result;

    for (Uint32 i = 0, n = str.size(); i < n; i++)
    {
	Char16 c = str[i];

	if (c == '\\')
	{
	    if (i + 1 == n)
		break;

	    c = str[i + 1];

	    switch (c)
	    {
		case 'n':
		    result.append("\n");
		    break;

		case 'r':
		    result.append("\r");
		    break;

		case 't':
		    result.append("\t");
		    break;

		case 'f':
		    result.append("\f");
		    break;

		default:
		    result.append(c);
	    }

	    i++;
	}
	else
	    result.append(c);
    }

    return result;
}

static Boolean _GetRecord(ifstream& is, Array<String>& fields)
{
    fields.clear();
    String line;

    for (Uint32 i = 0; i < NUM_FIELDS; i++)
    {
	if (!GetLine(is, line))
	    return false;

        fields.append(line);
        //Association names are not supposed to contain escapes
        //fields.append(_Unescape(line));
    }

    // Skip the blank line:

    if (!GetLine(is, line))
	return false;

    return true;
}

static void _PutRecord(ofstream& os, Array<String>& fields)
{
    for (Uint32 i = 0, n = fields.size(); i < n; i++)
    {
        // Calling getCString to ensure utf-8 goes to the file
        // Calling write to ensure no data conversion by the stream
        CString  buffer = _Escape(fields[i]).getCString();
        os.write((const char *)buffer,
            static_cast<streamsize>(strlen((const char *)buffer)));
        os << endl;
    }
    os << endl;
}

void AssocClassTable::append(
    PEGASUS_STD(ofstream)& os,
    const String& path,
    const CIMName& assocClassName,
    const CIMName& fromClassName,
    const CIMName& fromPropertyName,
    const CIMName& toClassName,
    const CIMName& toPropertyName)
{
    Array<String> fields;
    fields.reserveCapacity(5);
    fields.append(assocClassName.getString());
    fields.append(fromClassName.getString());
    fields.append(fromPropertyName.getString());
    fields.append(toClassName.getString());
    fields.append(toPropertyName.getString());

    for (Uint16 x=0; x<fields.size();x++)
    {
        fields[x].toLower();
    }

    _PutRecord(os, fields);

    // Update cache
    AssocClassCache *cache = AssocClassCache::getAssocClassCache(path);
    if (cache != 0)
    {
        if (cache->isActive())
        {
            WriteLock lock(_classCacheLock);
            cache->addRecord(fields[FROM_CLASS_NAME_INDEX],
                             fields);
        }
    }
}

void AssocClassTable::append(
    const String& path,
    const CIMName& assocClassName,
    const CIMName& fromClassName,
    const CIMName& fromPropertyName,
    const CIMName& toClassName,
    const CIMName& toPropertyName)
{
    // Open input file:

    ofstream os;

    if (!OpenAppend(os, path))
	throw CannotOpenFile(path);

    // Insert the entry:

    Array<String> fields;
    fields.reserveCapacity(5);
    fields.append(assocClassName.getString());
    fields.append(fromClassName.getString());
    fields.append(fromPropertyName.getString());
    fields.append(toClassName.getString());
    fields.append(toPropertyName.getString());

    for (Uint16 x=0; x<fields.size();x++)
    {
        fields[x].toLower();
    }

    _PutRecord(os, fields);

    // Update cache
    AssocClassCache *cache = AssocClassCache::getAssocClassCache(path);
    if (cache != 0)
    {
        if (cache->isActive())
        {
            WriteLock lock(_classCacheLock);
            cache->addRecord(fields[FROM_CLASS_NAME_INDEX],
                             fields);
        }
    }
}

Boolean AssocClassTable::deleteAssociation(
    const String& path,
    const CIMName& assocClassName)
{
    // Open input file:

    ifstream is;

    if (!Open(is, path))
	return false;

    // Open output file:

    String tmpPath = path + ".tmp";
    ofstream os;

    if (!Open(os, tmpPath))
	throw CannotOpenFile(tmpPath);

    // Copy over all lines except ones with the given association instance name:

    Array<String> fields;
    Array<String> fieldsToDelete;
    Boolean found = false;

    while (_GetRecord(is, fields))
    {
	if (assocClassName.getString() != fields[ASSOC_CLASS_NAME_INDEX])
	{
	    _PutRecord(os, fields);
	    found = true;
	}
        else
        {
            fieldsToDelete = fields;
        }
    }

    // Close both files:

    is.close();
    os.close();

    // Remove orginal file:

    if (!FileSystem::removeFile(path))
	throw CannotRemoveFile(path);

    // Rename back to original name:

    if (!FileSystem::renameFile(tmpPath, path))
	throw CannotRenameFile(path);


    // Update cache
    if (found)
    {
        AssocClassCache *cache = AssocClassCache::getAssocClassCache(path);
        if (cache != 0)
        {
            if (cache->isActive())
            {
                WriteLock lock(_classCacheLock);
                cache->removeRecord(fieldsToDelete[FROM_CLASS_NAME_INDEX],
                                    fieldsToDelete[ASSOC_CLASS_NAME_INDEX]);
            }
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

    if (!Open(is, path))
        return false;

    Array<String> fields;
    Boolean found = false;

    // For each line in the associations table:
    while (_GetRecord(is, fields))
    {
        // Process associations from the right end class and with right roles
        if (_ContainsClass(classList, fields[FROM_CLASS_NAME_INDEX]) &&
            _MatchNoCase(fields[FROM_PROPERTY_NAME_INDEX], role) &&
            _MatchNoCase(fields[TO_PROPERTY_NAME_INDEX], resultRole))
        {
            // Skip classes that do not appear in the association class list
            if ((assocClassList.size() != 0) &&
                (!_ContainsClass(assocClassList,
                                 fields[ASSOC_CLASS_NAME_INDEX])))
            {
                continue;
            }

            // Skip classes that do not appear in the result class list
            if ((resultClassList.size() != 0) &&
                (!_ContainsClass(resultClassList,
                                 fields[TO_CLASS_NAME_INDEX])))
            {
                continue;
            }

            // This class qualifies; add it to the list (skipping duplicates)
            if (!Contains(associatorNames, fields[TO_CLASS_NAME_INDEX]))
            {
                associatorNames.append(fields[TO_CLASS_NAME_INDEX]);
            }
            found = true;
        }
    }

    return found;
}

Boolean AssocClassTable::_InitializeCache( AssocClassCache *cache,
                                           const String& path)
{
    WriteLock lock(_classCacheLock);

    if (!cache->isActive())
    {

        // Open input file:
        ifstream is;

        if (!Open(is, path))
            return false;

        Array<String> fields;

        // For each line in the associations table:
        while (_GetRecord(is, fields))
        {
            // The cache key is always expected to be in lowercase
            // While new association records are added in lowercase,
            // the following line is required for compatibility
            // with old repositories.
            fields[FROM_CLASS_NAME_INDEX].toLower();
            cache->addRecord(fields[FROM_CLASS_NAME_INDEX],
                             fields);
        }

        cache->setActive(true);
    }

    return true;
}

void AssocClassTable::removeCaches()
{
    WriteLock lock(_classCacheLock);
    AssocClassCache::cleanupAssocClassCaches();

    return;
}

Boolean AssocClassTable::getReferenceNames(
    const String& path,
    const Array<CIMName>& classList,
    const Array<CIMName>& resultClassList,
    const String& role,
    Array<String>& referenceNames)
{

    // First see if we can get the information from the association class cache.
    AssocClassCache *cache = AssocClassCache::getAssocClassCache(path);
    if (cache == 0)
        return false;

    if (!cache->isActive())
    {
        if (!_InitializeCache(cache,path))
            return false;
    }

    Array< Array<String> > records;
    Boolean found = false;


    // For each of the target classes retrieve the list of matching
    // association classes from the cache.
    // The cache uses the from class name as an index and returns all
    // association class records having that from class.

    ReadLock lock(_classCacheLock);
    for (Uint16 idx=0; idx < classList.size(); idx++)
    {
        String fromClassName = classList[idx].getString();
        fromClassName.toLower();
        if (cache->getAssocClassEntry(fromClassName, records))
        {
            for (Uint16 rx=0; rx <records.size(); rx++)
            {
                if (_MatchNoCase(records[rx][FROM_PROPERTY_NAME_INDEX], role))
        {
            // Skip classes that do not appear in the result class list
            if ((resultClassList.size() != 0) &&
                (!_ContainsClass(resultClassList,
                                         records[rx][ASSOC_CLASS_NAME_INDEX])))
            {
                continue;
            }

            // This class qualifies; add it to the list (skipping duplicates)
                    if (!Contains(referenceNames, records[rx][ASSOC_CLASS_NAME_INDEX]))
            {
                        referenceNames.append(records[rx][ASSOC_CLASS_NAME_INDEX]);
            }
            found = true;
        }
    }
        }
    }

    return found;
}

PEGASUS_NAMESPACE_END

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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <fstream>
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

static inline Boolean _MatchNoCase(const String& x, const String& pattern)
{
    return pattern.size() == 0 || String::equalNoCase(x, pattern);
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
		result += "\\n";
		break;

	    case '\r':
		result += "\\r";
		break;

	    case '\t':
		result += "\\t";
		break;

	    case '\f':
		result += "\\f";
		break;

	    case '\\':
		result += "\\\\";
		break;

	    default:
		result += c;
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
		    result += "\n";
		    break;

		case 'r':
		    result += "\r";
		    break;

		case 't':
		    result += "\t";
		    break;

		case 'f':
		    result += "\f";
		    break;

		default:
		    result += c;
	    }

	    i++;
	}
	else
	    result += c;
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

	fields.append(_Unescape(line));
    }

    // Skip the blank line:

    if (!GetLine(is, line))
	return false;

    return true;
}

static void _PutRecord(ofstream& os, Array<String>& fields)
{
    for (Uint32 i = 0, n = fields.size(); i < n; i++)
	os << _Escape(fields[i]) << endl;
    os << endl;
}

void AssocClassTable::append(
    PEGASUS_STD(ofstream)& os,
    const String& assocClassName,
    const String& fromClassName,
    const String& fromPropertyName,
    const String& toClassName,
    const String& toPropertyName)
{
    Array<String> fields;
    fields.reserveCapacity(5);
    fields.append(assocClassName);
    fields.append(fromClassName);
    fields.append(fromPropertyName);
    fields.append(toClassName);
    fields.append(toPropertyName);

    _PutRecord(os, fields);
}

void AssocClassTable::append(
    const String& path,
    const String& assocClassName,
    const String& fromClassName,
    const String& fromPropertyName,
    const String& toClassName,
    const String& toPropertyName)
{
    // Open input file:
    
    ofstream os;

    if (!OpenAppend(os, path))
	throw CannotOpenFile(path);

    // Insert the entry:

    Array<String> fields;
    fields.reserveCapacity(5);
    fields.append(assocClassName);
    fields.append(fromClassName);
    fields.append(fromPropertyName);
    fields.append(toClassName);
    fields.append(toPropertyName);

    _PutRecord(os, fields);
}

Boolean AssocClassTable::deleteAssociation(
    const String& path,
    const String& assocClassName)
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
    Boolean found = false;

    while (_GetRecord(is, fields))
    {
	if (assocClassName != fields[ASSOC_CLASS_NAME_INDEX])
	{
	    _PutRecord(os, fields);
	    found = true;
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

    return found;
}

Boolean AssocClassTable::getAssociatorNames(
    const String& path,
    const String& className,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Array<String>& associatorNames)
{
    // Open input file:
    
    ifstream is;

    if (!Open(is, path))
	return false;

    // For each line:

    Array<String> fields;
    Boolean found = false;


    while (_GetRecord(is, fields))
    {
	if (_MatchNoCase(className, fields[FROM_CLASS_NAME_INDEX]) &&
	    _MatchNoCase(fields[ASSOC_CLASS_NAME_INDEX], assocClass) &&
	    _MatchNoCase(fields[TO_CLASS_NAME_INDEX], resultClass) &&
	    _MatchNoCase(fields[FROM_PROPERTY_NAME_INDEX], role) &&
	    _MatchNoCase(fields[TO_PROPERTY_NAME_INDEX], resultRole))
	{
	    associatorNames.append(fields[TO_CLASS_NAME_INDEX]);
	    found = true;
	}
    }

    return found;
}

Boolean AssocClassTable::getReferenceNames(
    const String& path,
    const String& className,
    const String& resultClass,
    const String& role,
    Array<String>& referenceNames)
{
    // Open input file:
    
    ifstream is;

    if (!Open(is, path))
	return false;

    // For each line:

    Array<String> fields;
    Boolean found = false;

    while (_GetRecord(is, fields))
    {
	if (_MatchNoCase(className, fields[FROM_CLASS_NAME_INDEX]) &&
	    _MatchNoCase(fields[ASSOC_CLASS_NAME_INDEX], resultClass) &&
	    _MatchNoCase(fields[FROM_PROPERTY_NAME_INDEX], role))
	{
	    if (!Contains(referenceNames, fields[ASSOC_CLASS_NAME_INDEX]))
		referenceNames.append(fields[ASSOC_CLASS_NAME_INDEX]);
	    found = true;
	}
    }

    return found;
}

PEGASUS_NAMESPACE_END

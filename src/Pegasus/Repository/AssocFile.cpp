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

#include <fstream>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include "AssocFile.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define ASSOC_INSTANCE_NAME_INDEX 0
#define ASSOC_CLASS_NAME_INDEX 1
#define FROM_OBJECT_NAME_INDEX 2
#define FROM_CLASS_NAME_INDEX 3
#define FROM_PROPERTY_NAME_INDEX 4
#define TO_OBJECT_NAME_INDEX 5
#define TO_CLASS_NAME_INDEX 6 
#define TO_PROPERTY_NAME_INDEX 7

static inline Boolean _Match(const String& x, const String& pattern)
{
    return pattern.size() == 0 || x == pattern;
}

static Boolean _GetLine(ifstream& is, Array<String>& fields)
{
    // Get the next line:

    String line;

    if (!GetLine(is, line))
	return false;

    // Split line into fields:

    String::split(line, fields);
    return true;
}

static void _WriteLine(
    ofstream& os,
    Array<String>& fields)
{
    // Insert the entry:

    String line;
    String::join(fields, line);
    os << line << endl;
}

void AssocFile::append(
    const String& path,
    const String& assocInstanceName,
    const String& assocClassName,
    const String& fromObjectName,
    const String& fromClassName,
    const String& fromPropertyName,
    const String& toObjectName,
    const String& toClassName,
    const String& toPropertyName)
{
    // Open input file:
    
    ofstream os;

    if (!OpenAppend(os, path))
	throw CannotOpenFile(path);

    // Insert the entry:

    Array<String> fields;
    fields.reserve(8);
    fields.append(assocInstanceName);
    fields.append(assocClassName);
    fields.append(fromObjectName);
    fields.append(fromClassName);
    fields.append(fromPropertyName);
    fields.append(toObjectName);
    fields.append(toClassName);
    fields.append(toPropertyName);

    _WriteLine(os, fields);
}

Boolean AssocFile::containsObject(
    const String& path,
    const String& objectName)
{
    // Open input file:

    ifstream is;

    if (!Open(is, path))
	throw CannotOpenFile(path);

    // Look at each line:

    Array<String> fields;

    while (_GetLine(is, fields))
    {
	if (fields[TO_OBJECT_NAME_INDEX] == objectName)
	    return true;
    }

    return false;
}

Boolean AssocFile::deleteAssociation(
    const String& path,
    const String& assocInstanceName)
{
    // Open input file:

    ifstream is;

    if (!Open(is, path))
	throw CannotOpenFile(path);

    // Open output file:

    String tmpPath = path + ".tmp";
    ofstream os;

    if (!Open(os, tmpPath))
	throw CannotOpenFile(tmpPath);

    // Copy over all lines except ones with the given association instance name:

    Array<String> fields;
    Boolean found = false;

    while (_GetLine(is, fields))
    {
	if (assocInstanceName != fields[ASSOC_INSTANCE_NAME_INDEX])
	{
	    _WriteLine(os, fields);
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

Boolean AssocFile::getAssociatorNames(
    const String& path,
    const String& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Array<String>& associatorNames)
{
    // Open input file:
    
    ifstream is;

    if (!Open(is, path))
	throw CannotOpenFile(path);

    // For each line:

    Array<String> fields;
    Boolean found = false;

    while (_GetLine(is, fields))
    {
	if (objectName == fields[FROM_OBJECT_NAME_INDEX] &&
	    _Match(fields[ASSOC_CLASS_NAME_INDEX], assocClass) &&
	    _Match(fields[TO_CLASS_NAME_INDEX], resultRole) &&
	    _Match(fields[FROM_PROPERTY_NAME_INDEX], role) &&
	    _Match(fields[TO_PROPERTY_NAME_INDEX], resultRole))
	{
	    associatorNames.append(fields[TO_OBJECT_NAME_INDEX]);
	    found = true;
	}
    }

    return found;
}

Boolean AssocFile::getReferenceNames(
    const String& path,
    const String& objectName,
    const String& resultClass,
    const String& role,
    Array<String>& referenceNames)
{
    // Open input file:
    
    ifstream is;

    if (!Open(is, path))
	throw CannotOpenFile(path);

    // For each line:

    Array<String> fields;
    Boolean found = false;

    while (_GetLine(is, fields))
    {
	if (objectName == fields[FROM_OBJECT_NAME_INDEX] &&
	    _Match(fields[ASSOC_CLASS_NAME_INDEX], resultClass) &&
	    _Match(fields[FROM_PROPERTY_NAME_INDEX], role))
	{
	    referenceNames.append(fields[ASSOC_INSTANCE_NAME_INDEX]);
	    found = true;
	}
    }

    // Get rid of duplicates:

    BubbleSort(referenceNames);
    Unique(referenceNames);

    return found;
}

PEGASUS_NAMESPACE_END

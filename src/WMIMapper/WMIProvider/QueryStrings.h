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
// Author: Barbara Packard (barbara_packard@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WMIQueryStrings_h
#define Pegasus_WMIQueryStrings_h

/** This file contains definitions of string constants used to define queries 
	that will be used to process the Associator and Reference requests.
*/

PEGASUS_NAMESPACE_BEGIN

#define qString(x)		String(QUERY_STRING_LIST[x])
#define qChar(x)		QUERY_STRING_LIST[x]

static char const* QUERY_STRING_LIST[] =
{
	"ASSOCIATORS OF {%s}" ,
	" WHERE",
	" ClassDefsOnly",
	" AssocClass = ",
	" RequiredAssocQualifier = ",
	" RequiredQualifier = ",
	" ResultClass = ",
	" ResultRole = ",
	" Role = %s",
	" SchemaOnly",
	"REFERENCES OF {%s}",
	"WQL",
	":",
	".",
	"=R\"",
	"\\\"",
	"\"",
	"/"
};

// Constants identifying the strings

enum QUERY_STRING_ID
{
	Q_ASSOCIATORS,
	Q_WHERE,
	Q_CLS_DEFS,
	Q_ASSOC_CLS,
	Q_REQ_ASSOC_QUAL,
	Q_REQ_QUAL,
	Q_RESULT_CLASS,
	Q_RESULT_ROLE,
	Q_ROLE,
	Q_SCHEMA,
	Q_REFERENCES,
	Q_WQL,
	Q_COLON,
	Q_PERIOD,
	Q_REF_KEY,
	Q_SLASH_QUOTE,
	Q_QUOTE,
	Q_SLASH

};



PEGASUS_NAMESPACE_END

#endif /* Pegasus_WMIQueryStrings_h */

//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_QueryException_h
#define Pegasus_QueryException_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Query/QueryCommon/Linkage.h>
#include <Pegasus/Common/Exception.h>
//#include <Pegasus/Common/MessageLoader.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN
                 
class PEGASUS_QUERYCOMMON_LINKAGE QueryException : public Exception
{
	public:
		static const char MSG[];
                static const char KEY[];
		QueryException(const String& msg):
			Exception(MessageLoaderParms(KEY,MSG,msg)){}
		QueryException(const ContentLanguages& langs, const String& msg):
			Exception(msg) { setContentLanguages(langs); }
		QueryException(const ContentLanguages& langs, const MessageLoaderParms& parms):
                        Exception(parms) { setContentLanguages(langs); }
		QueryException(MessageLoaderParms parms):
			Exception(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE QueryLanguageInvalidException : public QueryException
{
	public:
		static const char MSG[];
		static const char KEY[];
      QueryLanguageInvalidException(const String& msg):
        QueryException(MessageLoaderParms(KEY,MSG,msg)){}
		QueryLanguageInvalidException(MessageLoaderParms parms):
                        QueryException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLParseException : public QueryException
{
   	public:
		static const char MSG[];
                static const char KEY[];
                CQLParseException(const String& msg):
			QueryException(MessageLoaderParms(KEY,MSG,msg)){}
		CQLParseException(MessageLoaderParms parms):
			QueryException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLRuntimeException : public QueryException
{
	public:
		static const char MSG[];
                static const char KEY[];
                CQLRuntimeException(const String& msg):
			QueryException(MessageLoaderParms(KEY,MSG,msg)){}
		CQLRuntimeException(MessageLoaderParms parms):
			QueryException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLValidationException : public QueryException
{
	public:
		static const char MSG[];
                static const char KEY[];
                CQLValidationException(const String& msg):
			QueryException(MessageLoaderParms(KEY,MSG,msg)){}
		CQLValidationException(MessageLoaderParms parms):
			QueryException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLSyntaxErrorException : public CQLParseException
{
	public:
		static const char MSG[];
                static const char KEY[];
                CQLSyntaxErrorException(const String& msg, Uint32 token, Uint32 position, const String& rule):
			CQLParseException(MessageLoaderParms(KEY,MSG,msg,token, position,rule)){}
		CQLSyntaxErrorException(MessageLoaderParms parms):
                        CQLParseException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLIdentifierParseException : public CQLParseException
{
	public:
		static const char MSG[];
      static const char KEY[];
      CQLIdentifierParseException(const String& msg):
		CQLParseException(MessageLoaderParms(KEY,MSG,msg)){}
		CQLIdentifierParseException(MessageLoaderParms parms):
                        CQLParseException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLChainedIdParseException : public CQLParseException
{
	public:
		static const char MSG[];
		static const char KEY[];
      CQLChainedIdParseException(const String& msg):
      CQLParseException(MessageLoaderParms(KEY,MSG,msg)){}
		CQLChainedIdParseException(MessageLoaderParms parms):
                        CQLParseException(parms){}
};

class PEGASUS_QUERYCOMMON_LINKAGE CQLNullContagionException : public CQLRuntimeException
{
	public:
		static const char MSG[];
		static const char KEY[];
      CQLNullContagionException(const String& msg):
        CQLRuntimeException(MessageLoaderParms(KEY,MSG,msg)){}
		CQLNullContagionException(MessageLoaderParms parms):
                        CQLRuntimeException(parms){}
};

// Macros to convert non QueryExceptions to QueryExceptions

#define PEGASUS_QUERY_EXCEPTION(LANGS, MESSAGE) QueryException( LANGS, MESSAGE )

#define PEGASUS_QUERY_EXCEPTION_LANG(LANGS, PARMS) QueryException( LANGS, PARMS )

PEGASUS_NAMESPACE_END
#endif
#endif

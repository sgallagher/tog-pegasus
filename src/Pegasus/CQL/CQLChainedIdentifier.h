
//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L. P.;
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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CQLChainedIdentifier_h
#define Pegasus_CQLChainedIdentifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN 
class PEGASUS_CQL_LINKAGE CQLScope;
class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLChainedIdentifierRep;
/**  
CQLChainedIdentifier class is used for parsing (not resolving)
the identifier portion of a CQL select statement.  
A couple examples of a CQLIdentifier:

  Class.EmbeddedObject.Property[3]
  Class.Property#'OK'


The CQLChainedIdentifier class can parse and return each
 section of a CQL identifier.  The possible sections are:

     class (optional)
     zero or more embedded objects
     property

Note:  this must handle wildcard("*"), embedded objects, arrays, symbolic
constants, etc.
   */
class PEGASUS_CQL_LINKAGE CQLChainedIdentifier
{
  public:

    CQLChainedIdentifier();
    /**  constructor 
          Parses the string into the various components of a CQL identifier.
          Throws parsing errors.
      */
    CQLChainedIdentifier(String inString);

    CQLChainedIdentifier(CQLIdentifier & id);
 
    CQLChainedIdentifier(const CQLChainedIdentifier& cid);

    ~CQLChainedIdentifier();

    /**  The getSubIdentifiers method
    returns the array of subIdentifiers from a CQL SELECT
    statement.  If there are no subidentifiers, an empty array is returned.
    
    Example:
        myClass.embeddedObject.myProperty would return an array of 2 strings:
    'myClass' and 'embeddedObject'.
        myProperty would return an empty array.
    
      */
    const Array<CQLIdentifier>& getSubIdentifiers()const;

    String toString()const;

    void append(CQLIdentifier & id);

    Uint32 size();

    Boolean prepend(CQLIdentifier & id);

    CQLIdentifier& operator[](Uint32 index);

    CQLChainedIdentifier& operator=(const CQLChainedIdentifier& rhs);

    Boolean isSubChain(CQLChainedIdentifier & chain);

    CQLIdentifier getLastIdentifier();
	
    void applyContext(QueryContext& inContext);

    friend class CQLFactory;

  private:

    void parse(String & string);

    CQLChainedIdentifierRep* _rep;

};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLChainedIdentifier
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CQLChainedIdentifier_h */

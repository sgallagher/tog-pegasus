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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com) 
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <cassert>
                                                                                                                                       
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/CQL/CQLIdentifier.h>   
#include <Pegasus/Query/QueryCommon/SubRange.h>

PEGASUS_USING_PEGASUS;
                                                                                                                                       
PEGASUS_USING_STD;

void print(CQLIdentifier &_id){
	cout << "Name = " << _id.getName().getString() << endl;
	if(_id.isScoped())
		cout << "Scope = " << _id.getScope() << endl;
	if(_id.isSymbolicConstant())
		cout << "Symbolic Constant = " << _id.getSymbolicConstantName() << endl;
	if(_id.isWildcard())
		cout << "CQLIdentifier = *" << endl;
	if(_id.isArray()){
		cout << "SubRanges: ";
		Array<SubRange> _ranges = _id.getSubRanges();
		for(Uint32 i = 0; i < _ranges.size(); i++){
			cout << _ranges[i].toString() << ",";
		}
		cout << endl;
	}
}

void drive_CQLIdentifier(){
	CQLIdentifier _ID1("ID1");
	assert(_ID1.getName() == "ID1");

	CQLIdentifier _ID2("ID2");
	assert(_ID1 != _ID2);

	CQLIdentifier _ID3("*");
	assert(_ID3.isWildcard());
	
	CQLIdentifier scopedID("SCOPE::IDENTIFIER");
	assert(scopedID.isScoped());
   assert(scopedID.getScope() == "SCOPE");

	CQLIdentifier _ID4("A::Name");
	CQLIdentifier _ID4a("A::Name");
	assert(_ID4 == _ID4a);
	
	CQLIdentifier symbolicConstantID("Name#OK");
	assert(symbolicConstantID.getName() == "Name");
	assert(symbolicConstantID.isSymbolicConstant());
	assert(symbolicConstantID.getSymbolicConstantName() == "OK");

   try{
	CQLIdentifier rangeID("SCOPE::Name[5,6,7]");
	// Basic query check
	assert(false);
	assert(rangeID.getName() == "Name");
        assert(rangeID.isArray());
        Array<SubRange> subRanges = rangeID.getSubRanges();
   	assert(subRanges[0] == String("5"));
   	assert(subRanges[1] == String("6"));
   	assert(subRanges[2] == String("7"));
        assert(rangeID.getScope() == "SCOPE");
   }catch(CQLIdentifierParseException& e){
   }

   try 
   {
     CQLIdentifier rangeID("SCOPE::Name[5..,6..,..7,4-5,..]");
     assert(false);
   }
   catch (QueryParseException & e)
   {
   }

   try 
   {
     CQLIdentifier rangeID1("Name[*]");
     assert(false);
   }
   catch (QueryParseException & e)
   {
   }

   try
   {
     CQLIdentifier invalid("Name#OK[4-5]");
     assert(false);
   }
   catch (CQLIdentifierParseException & e)
   {
   }


   try
   {  
     CQLIdentifier invalid1("Name[4-5]#OK");
     assert(false);
   }
   catch (CQLIdentifierParseException & e)
   {
   }
}

void drive_CQLChainedIdentifier()
{
  // Error cases
  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[*]");
   assert(false);
  }
  catch (QueryParseException & e)
  {
    // do nothing, exception is expected due to wildcard
  }

  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[1,3-5,7]");
   assert(false);
  }
  catch (QueryParseException & e)
  {
    // do nothing, exception is expected due to range
  }

  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[1..3]");
   assert(false);
  }
  catch (QueryParseException & e)
  {
    // do nothing, exception is expected due to range
  }

  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[]");
   assert(false);
  }
  catch (QueryParseException & e)
  {
    // do nothing, exception is expected due to missing index
  }

  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[3]#'ok'");
   assert(false);
  }
  catch (CQLIdentifierParseException & e)
  {
    // do nothing, exception is expected due to combo of array
    // and symbolic constant.
  }

  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[3");
   assert(false);
  }
  catch (CQLIdentifierParseException & e)
  {
    // do nothing, exception is expected due to missing ']'
  }

  try 
  {
	CQLChainedIdentifier _CI("CLASS.B::EO.A::this-is-bogus");
   assert(false);
  }
  catch (CQLIdentifierParseException & e)
  {
    // do nothing, exception is expected due to missing bad prop name
  }

  // Good case with all the bells and whistles, except wildcard
  //CQLChainedIdentifier _CI("FROMCLASS.SCOPE1::EO1.SCOPE2::EO2[1,3,5,7].SCOPE3::PROP#'ok'");
  CQLChainedIdentifier _CI("FROMCLASS.SCOPE1::EO1.SCOPE2::EO2[3].SCOPE3::PROP#'ok'");

  Array<CQLIdentifier> _arr = _CI.getSubIdentifiers();
  assert(_arr.size() == 4);

  assert(_arr[0].getName() == "FROMCLASS");
  assert(!_arr[0].isScoped());
  assert(!_arr[0].isSymbolicConstant());
  assert(!_arr[0].isArray());
  assert(!_arr[0].isWildcard());

  assert(_arr[1].getName() == "EO1");
  assert(_arr[1].isScoped());
  assert(_arr[1].getScope() == "SCOPE1");
  assert(!_arr[1].isSymbolicConstant());
  assert(!_arr[1].isArray());
  assert(!_arr[1].isWildcard());

  assert(_arr[2].getName() == "EO2");
  assert(_arr[2].isScoped());
  assert(_arr[2].getScope() == "SCOPE2");
  assert(!_arr[2].isSymbolicConstant());
  assert(_arr[2].isArray());
  Array<SubRange> ranges = _arr[2].getSubRanges();
  assert(ranges.size() == 1);
  //assert(ranges[0] == String("1"));
  assert(ranges[0] == String("3"));
  //assert(ranges[2] == String("5"));
  //assert(ranges[3] == String("7"));
  assert(!_arr[2].isWildcard());

  assert(_arr[3].getName() == "PROP");
  assert(_arr[3].isScoped());
  assert(_arr[3].getScope() == "SCOPE3");
  assert(_arr[3].isSymbolicConstant());
  assert(_arr[3].getSymbolicConstantName() == "'ok'");
  assert(!_arr[3].isArray());
  assert(!_arr[3].isWildcard());

  // Good case with all the bells and whistles, and wildcard
  CQLChainedIdentifier _CI1("FROMCLASS.SCOPE1::EO1.SCOPE2::EO2[7].*");

  _arr = _CI1.getSubIdentifiers();
  assert(_arr.size() == 4);

  assert(!_arr[3].isScoped());
  assert(!_arr[3].isSymbolicConstant());
  assert(!_arr[3].isArray());
  assert(_arr[3].isWildcard());

  try{
	CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[..3].SCOPE3::PROP#'ok'");
	assert(false);
  }catch(QueryParseException& e){
  }
  try{
        CQLChainedIdentifier _CI("CLASS.B::EO.A::PROP[3..].SCOPE3::PROP#'ok'");
        assert(false);
  }catch(QueryParseException& e){
  }
}

int main( int argc, char *argv[] ){

  //BEGIN TESTS....

  drive_CQLIdentifier();
  drive_CQLChainedIdentifier();

  //END TESTS....

  cout << argv[0] << " +++++ passed all tests" << endl;
                   
  return 0;
}


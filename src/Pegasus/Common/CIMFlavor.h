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

#ifndef Pegasus_Flavor_h
#define Pegasus_Flavor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

/** This structure defines flavor constants borne by CIMQualifier objects.
*/
struct PEGASUS_COMMON_LINKAGE CIMFlavor
{
    /** Indicates that the qualifier has no flavors.
    */
    static const Uint32 NONE;

    /** Indicates that the qualifier may be overriden.
    */
    static const Uint32 OVERRIDABLE;

    /** Indicates that the qualifier is propagated to the qualifier in the
	subclass with the same name.
    */
    static const Uint32 TOSUBCLASS;

    /** Indicates that the qualifier is propagated to the qualifier in the
	instance with the same name.
    */
    static const Uint32 TOINSTANCE;

    /** Indicates whether qualifier is translatable (for internationalization).
    */
    static const Uint32 TRANSLATABLE;

    /** Defaults according to the CIM DTD (OVERRIDABLE | TOSUBCLASS).
    */
    static const Uint32 DEFAULTS;

    /** Set both toInstance and toinstance according to the CIM DTD (OVERRIDABLE | TOSUBCLASS).
    */
    static const Uint32 TOSUBELEMENTS;
};

/** FlavorToMof - Converts the flavor attributes of a qualifier to CIM MOF
    keywords separated by spaces and returns them as a String.
    @param flavor Variable contianing the flavor mask
    @return Strin containing the flavor attribute keywords.
*/
PEGASUS_COMMON_LINKAGE String FlavorToMof(Uint32 flavor);

/** FlavorToXML - converts the flavor attributes of a qualifier to cimxml
    format and puts them into the variable out.
    @param out XML output stream into which the xml is places.
    @param flavor variable containing the flavor definition

	<pre>
	// Get flavorkeywords and test for any returned keywords
	Uint32 flavor = CIMFlavor::DEFAULTS,
	String flavorString;
	flavorString = FlavorToMof(_flavor);
	if (flavorString.size())
	    ...           // code to execute if keywords exist
	
	</pre>
*/
PEGASUS_COMMON_LINKAGE void FlavorToXml(
    Array<Sint8>& out, 
    Uint32 flavor);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Flavor_h */

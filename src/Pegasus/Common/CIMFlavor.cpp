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

#include <cstring>
#include "CIMFlavor.h"
#include "Exception.h"
#include "XmlWriter.h"
//#include <iostream>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

const Uint32 CIMFlavor::NONE = 0;
const Uint32 CIMFlavor::OVERRIDABLE = 1;
const Uint32 CIMFlavor::ENABLEOVERRIDE = 1;
const Uint32 CIMFlavor::TOSUBCLASS = 2;
const Uint32 CIMFlavor::TOINSTANCE = 4;
const Uint32 CIMFlavor::TRANSLATABLE = 8;
const Uint32 CIMFlavor::TOSUBELEMENTS = TOSUBCLASS | TOINSTANCE;
const Uint32 CIMFlavor::DISABLEOVERRIDE = 16;
const Uint32 CIMFlavor::RESTRICTED = 32;
const Uint32 CIMFlavor::DEFAULTS = OVERRIDABLE | TOSUBCLASS;
// ATTN: P1 KS 24 March 2002 Change here to make TOINSTANCE part of the defaults
//const Uint32 CIMFlavor::DEFAULTS = OVERRIDABLE | TOSUBCLASS| TOINSTANCE;
const Uint32 CIMFlavor::ALL = OVERRIDABLE | DEFAULTS
						 | TRANSLATABLE | DISABLEOVERRIDE | RESTRICTED;

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}
/** FlavorToMof	Convert the Qualifier flavors to a string of MOF 
    flavor keywords.

  <pre>
  Keyword            Function				  Default
    EnableOverride  Qualifier is overridable. 		    yes
    DisableOverride Qualifier cannot be overridden.          no
    ToSubclass      Qualifier is inherited by any subclass. yes
    Restricted      Qualifier applies only to the class	    no
                    in which it is declared
    Translatable    Indicates the value of the qualifier
                    can be specified inmultiple languages   no
    NOTE: There is an open issue with the keyword toinstance.

    flavor 	      = ENABLEOVERRIDE | DISABLEOVERRIDE | RESTRICTED |
			TOSUBCLASS | TRANSLATABLE
    DISABLEOVERRIDE   = "disableOverride"

    ENABLEOVERRIDE    = "enableoverride"

    RESTRICTED        = "restricted"

    TOSUBCLASS        = "tosubclass"

    TRANSLATABLE      = "translatable"
   </pre>
   The keyword toinstance is not in the CIMspecification. For the moment we are
   assuming that it is the same as the TOSubclass. We had a choice of using
   one entity for both or separating them and letting the compiler set both.
 
*/
String FlavorToMof(Uint32 flavor)
{
    Boolean overridable = (flavor & CIMFlavor::OVERRIDABLE) != 0;
    Boolean toSubClass = (flavor & CIMFlavor::TOSUBCLASS) != 0;
    Boolean toInstance = (flavor & CIMFlavor::TOINSTANCE) != 0;
    Boolean translatable = (flavor & CIMFlavor::TRANSLATABLE) != 0;

    String tmp;

    tmp = "";

    if (!overridable)
	tmp += "DisableOverride, ";

    if (!toSubClass)
	tmp += "Restricted, ";

    if (translatable)
	tmp += "Translatable, ";

    if (tmp.size())
	tmp.remove(tmp.size() - 2);

    return tmp;
}

/*
The XML Definition is, from XML Specification
<!ENTITY % QualifierFlavor "OVERRIDABLE  (true|false)   'true' 
                              TOSUBCLASS   (true|false)   'true' 
                              TOINSTANCE   (true|false)   'false' 
                              TRANSLATABLE (true|false)   'false'">
*/
void FlavorToXml(Array<Sint8>& out, Uint32 flavor)
{
    Boolean overridable = (flavor & CIMFlavor::OVERRIDABLE) != 0;
    Boolean toSubClass = (flavor & CIMFlavor::TOSUBCLASS) != 0;
    Boolean toInstance = (flavor & CIMFlavor::TOINSTANCE) != 0;
    Boolean translatable = (flavor & CIMFlavor::TRANSLATABLE) != 0;

    if (!overridable)
	out << " OVERRIDABLE=\"" << _toString(overridable) << "\"";

    /*cout << "KSTEST XML " << hex << flavor << "  overridable = " 
        << overridable 
        << " " << FlavorToMof(flavor)
        << endl; */

    if (!toSubClass)
	out << " TOSUBCLASS=\"" << _toString(toSubClass) << "\"";

    if (toInstance)
	out << " TOINSTANCE=\"" << _toString(toInstance) << "\"";

    if (translatable)
	out << " TRANSLATABLE=\"" << _toString(translatable) << "\"";
}


PEGASUS_NAMESPACE_END

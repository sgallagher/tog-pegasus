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
// Author: <Author's Name> (<Authors EMAIL>)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMEmbeddedObject.h"


#include <Pegasus/Common/Config.h>
//#include <cstdlib>
//#include <cstdio>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMInstance CIMEmbeddedObject::decodeEmbeddedObject(const String& xmlStr)
{
    CIMInstance inst;
    CString content(xmlStr.getCString());
    XmlParser parser((char*)(const char*)content);

    // Decode the Instance.  Note that a false
    // return indicates that there was no instance
    // decoded.
    if (!XmlReader::getInstanceElement(parser, inst))
        throw XmlValidationError(999,"No Instance found in CIMEmbeddedObject");
    return inst;

}

String CIMEmbeddedObject::encodeToEmbeddedObject (const CIMInstance& instance)

{
	static const String _lt("&lt;");
	static const String _gt("&gt;");
	static const String _amp("&amp;");
	static const String _quot("&quot;");
	static const String _apos("&apos;");
	static const String _num("&#");

    Array<Sint8> tmp;
    XmlWriter::appendInstanceElement(tmp,instance);
    tmp.append('\0');
    String rtnStr(tmp.getData());
    return(rtnStr);
}
PEGASUS_NAMESPACE_END

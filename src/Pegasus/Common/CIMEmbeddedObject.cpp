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
Boolean _transformString(const String& input, String& output, const String& from, const String& to)
{
    String work = input;
    Uint32 index;

    // No replacements necessary. Return false
    if(work.find(from) == PEG_NOT_FOUND)
        return false;

    // Replace any instances of from with to
    while ((index = work.find(from)) != PEG_NOT_FOUND)
    {
        output.append(work.subString(0, index));
        output.append(to);
        work.remove(0, (index + from.size()));
    }
    output.append(work);
    return(true);
}
CIMInstance CIMEmbeddedObject::decodeEmbeddedObject(const String& xmlStr)
{
    CIMInstance inst;

    // Map any escaped quote characters since we have a differnece
    // between the platforms on the concept of escaping the quote 
    // character.
    String tmp;
    if (_transformString(xmlStr, tmp, "\\&quot;", "\""));

    else if(_transformString(xmlStr, tmp, "\\\"", "\""));

    else
        tmp = xmlStr;

    CString content(tmp.getCString());
    // decode the required characters
    //cout << "Content Before parser " << content << endl;
    XmlParser parser((char*)(const char*)content);

    //parser._substituteReferences((char*)(const char*)content);
    // Decode the Instance.  False
    // return indicates that there was no instance
    // decoded.
    if (!XmlReader::getInstanceElement(parser, inst))
        throw XmlValidationError(999,"No Instance found in CIMEmbeddedObject");
    return inst;

}

String CIMEmbeddedObject::encodeToEmbeddedObject (const CIMInstance& instance)

{
   /* static const String _lt("&lt;");
	static const String _gt("&gt;");
	static const String _amp("&amp;");
	static const String _quot("&quot;");
	static const String _apos("&apos;");
	static const String _num("&#"); */

    Array<Sint8> tmp;
    XmlWriter::appendInstanceElement(tmp,instance);
    tmp.append('\0');
    String rtnStr(tmp.getData());
    return(rtnStr);
}
/****
struct EntityReference
{
    const char* match;
    Uint32 length;
    char replacement;
};

// ATTN: Add support for more entity references
static EntityReference _references[] =
{
    { "&amp;", 5, '&' },
    { "&lt;", 4, '<' },
    { "&gt;", 4, '>' },
    { "&quot;", 6, '"' },
    { "&apos;", 6, '\'' }
};

static Uint32 _REFERENCES_SIZE = (sizeof(_references) / sizeof(_references[0]));

String CIMEmbeddedObject::unquote(const String& input)
{
    String return;

    // Check for entity reference
    // ATTN: Inefficient if many entity references are supported
    Uint32 i;
    for (i = 0; i < _REFERENCES_SIZE; i++)
    {
        Uint32 length = _references[i].length;
        const char* match = _references[i].match;

        if (strncmp(p, _references[i].match, length) == 0)
        {
            referenceChar = _references[i].replacement;
            referenceLength = length;
            break;
        }
    }

    if (i == _REFERENCES_SIZE)
    {
        // Didn't recognize the entity reference
        // ATTN: Is there a good way to say "unsupported"?
        throw XmlException(code, _line);
    }
}
*/
PEGASUS_NAMESPACE_END

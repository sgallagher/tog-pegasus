//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

String namespaceName;
static Boolean verbose = false;

//------------------------------------------------------------------------------
// ProcessValueObjectElement()
//
//     <!ELEMENT VALUE.OBJECT (CLASS|INSTANCE)>
//
// ATTN: Nothing handled but CLASS.
//
//------------------------------------------------------------------------------

Boolean ProcessValueObjectElement(CIMRepository& repository, XmlParser& parser)
{
    XmlEntry entry;

    if (!XmlReader::testStartTag(parser, entry, "VALUE.OBJECT"))
    {
        return false;
    }

    CIMClass cimClass, tmpClass;
    CIMQualifierDecl qualifierDecl;

    if (XmlReader::getClassElement(parser, cimClass))
    {
        if (verbose) 
        {
            cout << "Creating: class ";
            cout << cimClass.getClassName().getString() << endl;
        }

        try
        {
            repository.createClass(namespaceName, cimClass);
        }
        catch (Exception &e)
        {
            // Ignore if cimClass already exists
            if (e.getMessage() != cimClass.getClassName())
            {
                throw;
            }
        }
    }
    else if (XmlReader::getQualifierDeclElement(parser, qualifierDecl))
    {
        if (verbose) 
        {
            cout << "Creating: qualifier ";
            cout << qualifierDecl.getName().getString() << endl;
        }

        try
        {
            repository.setQualifier(namespaceName, qualifierDecl);
        }
        catch (Exception &e)
        {
            // Ignore if qualifierDecl already exists
            if (e.getMessage() != qualifierDecl.getName())
            {
                throw;
            }
        }
    }
    XmlReader::expectEndTag(parser, "VALUE.OBJECT");

    return true;
}

//------------------------------------------------------------------------------
// ProcessDeclGroupElement()
//
//     <!ELEMENT DECLGROUP ((LOCALNAMESPACEPATH|NAMESPACEPATH)?,
//         QUALIFIER.DECLARATION*,VALUE.OBJECT*)>
//
// ATTN: Nothing handled but VALUE.OBJECT:
//
//------------------------------------------------------------------------------

Boolean ProcessDeclGroupElement(CIMRepository& repository, XmlParser& parser)
{
    XmlEntry entry;

    if (!XmlReader::testStartTag(parser, entry, "DECLGROUP"))
    {
        return false;
    } 

    while (ProcessValueObjectElement(repository, parser))
        ;

    XmlReader::expectEndTag(parser, "DECLGROUP");

    return true;
}

//------------------------------------------------------------------------------
// ProcessDeclarationElement()
//
//     <!ELEMENT DECLARATION (DECLGROUP|DECLGROUP.WITHNAME|DECLGROUP.WITHPATH)*>
//
// ATTN: DECLGROUP.WITHNAME ande DECLGROUP.WITHPATH not handled:
//
//------------------------------------------------------------------------------

Boolean ProcessDeclarationElement(CIMRepository& repository, XmlParser& parser)
{
    XmlEntry entry;

    if (!XmlReader::testStartTag(parser, entry, "DECLARATION"))
    {
        return false;
    }

    while(ProcessDeclGroupElement(repository, parser))
        ;

    XmlReader::expectEndTag(parser, "DECLARATION");

    return true;
}

//------------------------------------------------------------------------------
// ProcessCimElement()
//
//     <!ELEMENT CIM (MESSAGE|DECLARATION)>
//     <!ATTLIST CIM
//         CIMVERSION CDATA #REQUIRED
//         DTDVERSION CDATA #REQUIRED>
//
// ATTN: only declarations are handled here!
//
//------------------------------------------------------------------------------

Boolean ProcessCimElement(CIMRepository& repository, XmlParser& parser)
{
    XmlEntry entry;

    if (!parser.next(entry) || entry.type != XmlEntry::XML_DECLARATION)
    {
        throw(parser.getLine(), "expected XML declaration");
    }

    if (!XmlReader::testStartTag(parser, entry, "CIM"))
    {
        return false;
    }
    String cimVersion;

    if (!entry.getAttributeValue("CIMVERSION", cimVersion))
    {
        throw XmlValidationError(parser.getLine(), 
            "missing CIM.CIMVERSION attribute");
    }

    String dtdVersion;

    if (!entry.getAttributeValue("DTDVERSION", dtdVersion))
    {
        throw XmlValidationError(parser.getLine(), 
            "missing CIM.DTDVERSION attribute");
    }

    if (!ProcessDeclarationElement(repository, parser))
    {
        throw XmlValidationError(parser.getLine(), 
            "Expected DECLARATION element");
    }

    XmlReader::expectEndTag(parser, "CIM");

    return true;
}

//------------------------------------------------------------------------------
//
// _processFile()
//
//------------------------------------------------------------------------------

static void _processFile(const char* repositoryRoot, const char* xmlFileName)
{
    // Create the parser:

    Buffer text;
    text.reserveCapacity(1024 * 1024);
    FileSystem::loadFileToMemory(text, xmlFileName);
    XmlParser parser((char*)text.getData());

    CIMRepository repository(repositoryRoot);
    try
    {
        repository.createNameSpace(namespaceName);
    }
    catch (Exception &e)
    {
        // Ignore if Namespace already exists
        if (e.getMessage () != namespaceName)
        {
            throw;
        }
    }

    if (!ProcessCimElement(repository, parser))
    {
        cerr << "CIM root element missing" << endl;
        exit(1);
    }
}

//------------------------------------------------------------------------------
//
// main()
//
//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] 
            << " repository-root xmlfile namespace" << endl;
        exit(1);
    }

    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;

    try
    {
        namespaceName = argv[3]; 
        cout << argv[0] << " loading " << argv[2] << " to namespace "
            << namespaceName << " into repository " << argv[1] << endl;
        _processFile(argv[1], argv[2]);
        cout << argv[0] << " loaded." << endl;
    }
            
    catch (const Exception& e)
    {
        cerr << e.getMessage() << endl;
        exit(1);
    }

    return 0;
}

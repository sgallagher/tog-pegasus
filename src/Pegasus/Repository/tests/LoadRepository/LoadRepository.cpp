
#include <fstream>
#include <iostream>
#include <cassert>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Repository/CIMRepository.h>

using namespace Pegasus;
using namespace std;

static const String CIMV20_NAMESPACE = "root/cimv20";
static const String ROOT_NAMESPACE = "root";

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
	return false;

    CIMClass cimClass;

    XmlReader::getClassElement(parser, cimClass);

    cout << "Creating " << cimClass.getClassName() << "..." << endl;

    repository.createClass(CIMV20_NAMESPACE, cimClass);

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
	return false;

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
	return false;

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
	throw(parser.getLine(), "expected XML declaration");

    if (!XmlReader::testStartTag(parser, entry, "CIM"))
	return false;

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

    Array<Sint8> text;
    text.reserve(1024 * 1024);
    FileSystem::loadFileToMemory(text, xmlFileName);
    text.append('\0');
    XmlParser parser((char*)text.getData());

    CIMRepository repository(repositoryRoot);
    repository.createNameSpace(CIMV20_NAMESPACE);
    repository.createNameSpace(ROOT_NAMESPACE);

    // Create the qualifiers:

    repository.createMetaQualifiers(CIMV20_NAMESPACE);
    repository.createMetaQualifiers(ROOT_NAMESPACE);

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
    if (argc != 3)
    {
	cerr << "Usage: " << argv[0] << " repository-root xmlfile" << endl;
	exit(1);
    }

    try
    {
	_processFile(argv[1], argv[2]);
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;	
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}

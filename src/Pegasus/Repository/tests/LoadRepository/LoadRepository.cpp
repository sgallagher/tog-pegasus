//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: LoadRepository.cpp,v $
// Revision 1.2  2001/01/25 02:11:18  mike
// added file which contains standard meta qualifiers
//
// Revision 1.1.1.1  2001/01/14 19:53:58  mike
// Pegasus import
//
//
//END_HISTORY

#include <fstream>
#include <iostream>
#include <cassert>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Repository/Repository.h>

using namespace Pegasus;
using namespace std;

static const String NAMESPACE = "root/cimv20";

//------------------------------------------------------------------------------
// ProcessValueObjectElement()
//
//     <!ELEMENT VALUE.OBJECT (CLASS|INSTANCE)>
//
// ATTN: Nothing handled but CLASS.
//
//------------------------------------------------------------------------------

Boolean ProcessValueObjectElement(Repository& repository, XmlParser& parser)
{
    XmlEntry entry;

    if (!XmlReader::testStartTag(parser, entry, "VALUE.OBJECT"))
	return false;

    ClassDecl classDecl;

    XmlReader::getClassElement(parser, classDecl);

    cout << "Creating " << classDecl.getClassName() << "..." << endl;

    repository.createClass(NAMESPACE, classDecl);

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

Boolean ProcessDeclGroupElement(Repository& repository, XmlParser& parser)
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

Boolean ProcessDeclarationElement(Repository& repository, XmlParser& parser)
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

Boolean ProcessCimElement(Repository& repository, XmlParser& parser)
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
// CreateQualifiers()
//
//------------------------------------------------------------------------------

void CreateQualifiers(Repository& repository)
{
    // ATTN: not in meta schema!
    {
    QualifierDecl q("cimtype", String(), 
	Scope::PROPERTY | Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Description : string = null, Scope(any), Flavor(translatable);
    {
    QualifierDecl q("description", String(), Scope::ANY, Flavor::TRANSLATABLE);
    repository.setQualifier(NAMESPACE, q);
    }

    // ATTN: the qualifier declaration in the meta schema does not match
    // usage in XML schema! (uint32 vs. sing32)

    // Qualifier MaxLen : uint32 = null, Scope(property, method, parameter);
    {
    QualifierDecl q(
	"maxlen", Sint32(0), Scope::PROPERTY|Scope::METHOD|Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Abstract : boolean = false, 
    // Scope(class, association, indication),
    {
    QualifierDecl q(
	"abstract", false, Scope::CLASS|Scope::ASSOCIATION|Scope::INDICATION);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier MappingStrings : string[],
    // Scope(class, property, association, indication, reference);
    {
    QualifierDecl q("mappingstrings", Array<String>(),
	Scope::CLASS|Scope::PROPERTY|Scope::ASSOCIATION|Scope::INDICATION|
	Scope::REFERENCE);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier ValueMap : string[], Scope(property, method, parameter);
    {
    QualifierDecl q("valuemap", Array<String>(), 
	Scope::PROPERTY|Scope::METHOD|Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Key : boolean = false, Scope(property, reference),
    // Flavor(disableoverride);
    {
    QualifierDecl q(
	"key", false, Scope::PROPERTY|Scope::REFERENCE, Flavor::NONE);
    repository.setQualifier(NAMESPACE, q);
    }

    {
    QualifierDecl q("id", Sint32(0), 
	Scope::PARAMETER|Scope::PROPERTY, Flavor::NONE);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier In : boolean = true, Scope(parameter), Flavor(disableoverride);
    {
    QualifierDecl q("in", true, Scope::PARAMETER, Flavor::NONE);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Out : boolean = false, Scope(parameter), 
    // Flavor(disableoverride);
    {
    QualifierDecl q("out", false, Scope::PARAMETER, Flavor::NONE);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Override : string = null, Scope(property, method, reference),
    // Flavor(disableoverride);
    {
    QualifierDecl q("override", String(), 
	Scope::PROPERTY|Scope::METHOD|Scope::REFERENCE,
	Flavor::TOSUBCLASS);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Values : string[], Scope(property, method, parameter),
    // Flavor(translatable);
    {
    QualifierDecl q("values", Array<String>(), 
	Scope::PROPERTY|Scope::METHOD|Scope::PARAMETER,
	Flavor::TRANSLATABLE);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier ArrayType : string = "Bag", Scope(property, parameter);
    {
    QualifierDecl q("arraytype", "Bag", Scope::PROPERTY|Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier ModelCorrespondence : string[], Scope(property);
    {
    QualifierDecl q("modelcorrespondence", Array<String>(), Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Units : string = null, Scope(property, method, parameter),
    // Flavor(translatable);
    {
    QualifierDecl q("units", String(), 
	Scope::PROPERTY|Scope::METHOD|Scope::PARAMETER,
	Flavor::TRANSLATABLE);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Counter : boolean = false, Scope(property, method, parameter);
    {
    QualifierDecl q("counter", false,
	Scope::PROPERTY|Scope::METHOD|Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Propagated : string = null, Scope(property, reference),
    // Flavor(disableoverride);
    {
    QualifierDecl q("propagated", String(),
	Scope::PROPERTY|Scope::REFERENCE, Flavor::TOSUBCLASS);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Required : boolean = false, Scope(property);
    {
    QualifierDecl q("required", false, Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // ATTN: missing!
    {
    QualifierDecl q("octetstring", false, Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Gauge : boolean = false, Scope(property, method, parameter);
    {
    QualifierDecl q(
	"gauge", false, Scope::PROPERTY|Scope::METHOD|Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Write : boolean = false, Scope(property);
    {
    QualifierDecl q("write", false, Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier MaxValue : sint64 = null, Scope(property, method, parameter);
    // ATTN: mismatch!
    {
    QualifierDecl q("maxvalue", Sint32(0), 
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier MinValue : sint64 = null, Scope(property, method, parameter);
    {
    QualifierDecl q("minvalue", Sint32(0), 
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Max : uint32 = null, Scope(reference);
    // ATTN: type and scope wrong!
    {
    QualifierDecl q("max", Sint32(0), Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Min : uint32 = null, Scope(reference);
    // ATTN: type and scope wrong!
    {
    QualifierDecl q("min", Sint32(0), Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Aggregate : boolean = false, Scope(reference),
    // Flavor(disableoverride, tosubclass);
    {
    QualifierDecl q("aggregate", false, Scope::PROPERTY);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Association : boolean = false, Scope(class, association),
    // Flavor(disableoverride);
    {
    QualifierDecl q("association", false, Scope::CLASS | Scope::ASSOCIATION, 
	Flavor::TOSUBCLASS);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Aggregation : boolean = false, Scope(association),
    // Flavor(disableoverride, tosubclass);
    {
    QualifierDecl q("aggregation", false, Scope::ASSOCIATION | Scope::CLASS,
	Flavor::TOSUBCLASS);
    repository.setQualifier(NAMESPACE, q);
    }

    // Qualifier Weak : boolean = false, Scope(reference),
    // Flavor(disableoverride, tosubclass);
    {
    QualifierDecl q("weak", false, Scope::PROPERTY, Flavor::TOSUBCLASS);
    repository.setQualifier(NAMESPACE, q);
    }
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

    Repository repository(repositoryRoot);
    repository.createNameSpace(NAMESPACE);

    // Create the qualifiers:

    CreateQualifiers(repository);

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

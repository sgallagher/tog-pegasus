Readme - LoadXml Utility Program

LoadXML - THis is a utility program primarily for test that loads
an XML file in the WBEM XML format into a repository.

This program is not intended for production use but in the test environment.

The command line is:

loadxml repository-root xmlfile namespace

It loads the xml formatted file defined by xmlfile into the namespace defined
by namespace in the repository defined by repository-root.

The format of an xml file is the same format as the XML defined in the
CIM Operations over HTTP specification.  This is the same format as the 
XML file created by the --xml parameter option on the Pegasus MOF compiler.

Karl Schopmeyer
15 February 2002
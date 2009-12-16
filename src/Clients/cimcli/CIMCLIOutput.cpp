//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/General/MofWriter.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/ArrayInternal.h>
#include "CIMCLIClient.h"
#include "CIMCLIOutput.h"
#include "CIMCLICommon.h"


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
//
// mofFormat
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os,
                    Uint32 level,
                    Uint32 indentSize)
{
    Uint32 n = level * indentSize;
    if (n > 50)
    {
        cout << "Jumped Ship " << level << " size " << indentSize << endl;
        exit(1);
    }

    for (Uint32 i = 0; i < n; i++)
    {
        os << ' ';
    }
}

/* Format the output stream for indented MOF format
*/
void mofFormat(PEGASUS_STD(ostream)& os,
    const char* text,
    Uint32 indentSize)
{
    char* var = new char[strlen(text)+1];
    char* tmp = strcpy(var, text);
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    Boolean qualifierState = false;
    char c;
    char prevchar = 0;
    while ((c = *tmp++))
    {
        count++;
        // This is too simplistic and must move to a token based mini parser
        // but will do for now. One problem is tokens longer than 12
        // characters that overrun the max line length.
        switch (c)
        {
            case '\n':
                os << c;
                prevchar = c;
                count = 0 + (indent * indentSize);
                _indent(os, indent, indentSize);
                break;

            case '\"':   // quote
                os << c;
                prevchar = c;
                quoteState = !quoteState;
                break;

            case ' ':
                os << c;
                prevchar = c;
                if (count > 66)
                {
                    if (quoteState)
                    {
                        os << "\"\n";
                        _indent(os, indent + 1, indentSize);
                        os <<"\"";
                    }
                    else
                    {
                        os <<"\n";
                        _indent(os, indent + 1,  indentSize);
                    }
                    count = 0 + ((indent + 1) * indentSize);
                }
                break;
            case '[':
                if (prevchar == '\n')
                {
                    indent++;
                    _indent(os, indent,  indentSize);
                    qualifierState = true;
                }
                os << c;
                prevchar = c;
                break;

            case ']':
                if (qualifierState)
                {
                    if (indent > 0)
                        indent--;
                    qualifierState = false;
                }
                os << c;
                prevchar = c;
                break;

            default:
                os << c;
                prevchar = c;
        }

    }
    delete [] var;
}

/*************************************************************
*
*       Common print functions for all CIM Objects Used
*
***************************************************************/

// output CIMProperty
void _print(const CIMProperty& property,
    const OutputType format)
{
    if (format == OUTPUT_XML)
    {
        XmlWriter::printPropertyElement(property,cout);
    }
    else if (format == OUTPUT_MOF)
    {
        CIMProperty pt = property.clone();
        pt.setPropagated(false);
        Buffer x;
        MofWriter::appendPropertyElement(false, x, pt);
        mofFormat(cout, x.getData(), 4);
    }
}

// output CIMValue in accord with output format definition
//NOTE: DUPLICATES OUTPUT FORMATT FUNCTION
void _print(const CIMValue& v,
    const OutputType format)
{
    if (format == OUTPUT_XML)
    {
        XmlWriter::printValueElement(v,cout);
    }
    else if (format == OUTPUT_MOF)
    {
        Buffer x;
        MofWriter::appendValueElement(x, v);
        mofFormat(cout, x.getData(), 4);
    }
    else
    {
        cout << " Format type error" << endl;
    }
}

/*****************************************************************************
*
*       Formatting and print functions for table output of instances
*
******************************************************************************/
//
//  Definition for String entries for each column in the output
//
typedef Array <String> ColumnEntry;

/* Output a single table String entry filling to ColSize or adding an
   eol if last specified
*/
void _printTableEntry(const String& entryStr,
    const Uint32 colSize,
    Boolean last,
    PEGASUS_STD(ostream)& outPrintWriter)
{
    Uint32 fillerLen = colSize - entryStr.size() + 2;

    outPrintWriter << entryStr;
    if (last)
    {
        outPrintWriter << endl;
    }
    else
    {
        for (Uint32 j = 0; j < fillerLen; j++)
        {
             outPrintWriter << ' ';
        }
    }
}

/* Print the formatted table form of the instances as defined by
   the parameters for the column width for each column and the array
   of column entries (outputTable).
*/
void _printTables(const Array<Uint32>& maxColumnWidth,
    const Array<ColumnEntry>& outputTable,
    PEGASUS_STD(ostream)& outPrintWriter)
{
    for (Uint32 i = 0; i < outputTable[0].size(); i++)
    {
        for (Uint32 column = 0; column < maxColumnWidth.size(); column++)
        {
            Boolean last = (column == maxColumnWidth.size() - 1);
            _printTableEntry(outputTable[column][i],
                maxColumnWidth[column],
                last,
                outPrintWriter);
        }
    }
}

/* Format the output stream to be a table with column for each property
   and row for the properties in each instance.
*/
void tableDisplay(PEGASUS_STD(ostream)& outPrintWriter,
    const Array<CIMInstance>& instances)
{
    Array<ColumnEntry> outputTable;
    Array<Uint32> maxColumnWidth;
    Array<String> propertyNameArray;

    // find set of all properties returned for all instances
    for (Uint32 i = 0; i < instances.size(); i++)
    {
        for (Uint32 j = 0; j < instances[i].getPropertyCount(); j++)
        {
            String propertyNameStr =
                instances[i].getProperty(j).getName().getString();

            // Add to outputTable if not already there
            if (!(Contains(propertyNameArray, propertyNameStr)))
            {
                //outputTable.append(propertyNameStr);
                maxColumnWidth.append(propertyNameStr.size());
                propertyNameArray.append(propertyNameStr);
            }
        }
    }

    // Build the complete table output in ascii.  We must build the
    // complete table to determine column widths.
    // NOTE: This code creates tables with column width to match the
    // maximum width of the string representation of the property name or
    // string representation of the value.  This can create REALLY
    // REALLY wide columns for Strings and for Array properties.
    //
    // TODO: Add code to create multiline colums for things like array
    // entries or possibly long strings.

    for (Uint32 i = 0; i < propertyNameArray.size(); i++)
    {
        // array for a single column of property values
        Array<String> propertyValueArray;

        String propertyNameStr = propertyNameArray[i];

        // First entry in propertyValueArray array is the propery name
        propertyValueArray.append(propertyNameStr);

        // for all instances get value for the property in
        // propertyNameArray
        for (Uint32 j = 0; j < instances.size(); j++)
        {
            Uint32 pos = instances[j].findProperty(propertyNameStr);

            // Get the value or empty string if there is no property
            // with this name
            String propertyValueStr = (pos != PEG_NOT_FOUND) ?
                    instances[j].getProperty(pos).getValue().toString()
                :
                    String::EMPTY;

            propertyValueArray.append(propertyValueStr);

            if (propertyValueStr.size() > maxColumnWidth[i])
            {
                maxColumnWidth[i] = propertyValueStr.size();
            }
        }

        // Append the value array for this property to the outputTable
        outputTable.append(propertyValueArray);
    }
    _printTables(maxColumnWidth, outputTable, outPrintWriter);
}

void _outputFormatInstance(const OutputType format,
    const CIMInstance& instance)
{
    // display the path element
    cout << "path= " << instance.getPath().toString() << endl;

    // Display the instance based on the format type.
    if (format == OUTPUT_XML)
    {
        XmlWriter::printInstanceElement(instance, cout);
    }
    else if (format == OUTPUT_MOF)
    {
        CIMInstance temp = instance.clone();
        // Reset the propagated flag to assure that these entities
        // are all shown in the MOF output.
        for (Uint32 i = 0 ; i < temp.getPropertyCount() ; i++)
        {
            CIMProperty p = temp.getProperty(i);
            p.setPropagated(false);
        }

        Buffer x;
        MofWriter::appendInstanceElement(x, temp);
        mofFormat(cout, x.getData(), 4);
    }
}
void _outputFormatParamValue(const OutputType format,
    const CIMParamValue& pv)
{
    if (format == OUTPUT_XML)
    {
        XmlWriter::printParamValueElement(pv, cout);
    }
    else if (format == OUTPUT_MOF)
    {
        if (!pv.isUninitialized())
        {
           CIMValue v =  pv.getValue();
           CIMType type = v.getType();
           if (pv.isTyped())
               cerr << cimTypeToString (type) << " ";
           else
               cerr << "UnTyped ";

           cerr << pv.getParameterName() << "="
                << v.toString() << endl;
        }
        else
            cerr << "ParamValue not initialized" << endl;
    }
    else
        cerr << "Error, Format Definition Error" << endl;
}

void _outputFormatClass(const OutputType format,
    const CIMClass& myClass)
{
    if (format == OUTPUT_XML)
        XmlWriter::printClassElement(myClass, cout);
    else if (format == OUTPUT_MOF)
    {
        // Reset the propagated flag to assure that these entities
        // are all shown in the MOF output.

        CIMClass temp = myClass.clone();
        for (Uint32 i = 0 ; i < temp.getPropertyCount() ; i++)
        {
            CIMProperty p = temp.getProperty(i);
            p.setPropagated(false);
        }
        for (Uint32 i = 0 ; i < temp.getMethodCount() ; i++)
        {
            CIMMethod m = temp.getMethod(i);
            m.setPropagated(false);
        }
        Buffer x;
        MofWriter::appendClassElement(x, temp);
        mofFormat(cout, x.getData(), 4);
    }
    else
        cerr << "Error, Format Definition Error" << endl;
}

void _outputFormatObject(const OutputType format,
    const CIMObject& myObject)
{

    if (myObject.isClass())
    {
        CIMClass c(myObject);
        _outputFormatClass(format, c);
    }
    else if (myObject.isInstance())
    {
        CIMInstance i(myObject);
        _outputFormatInstance(format, i);
    }
    else
    {
        cerr << "Error: Object is neither class or instance" << endl;
    }
}

void _outputFormatQualifierDecl(const OutputType format,
    const CIMQualifierDecl& myQualifierDecl)
{
    if (format == OUTPUT_XML)
        XmlWriter::printQualifierDeclElement(myQualifierDecl, cout);
    else if (format == OUTPUT_MOF)
    {
        Buffer x;
        MofWriter::appendQualifierDeclElement(x, myQualifierDecl);
        mofFormat(cout, x.getData(), 4);
    }
    else
    {
        cerr << "Error: Format type error" << endl;
    }
}

void _outputFormatCIMValue(const OutputType format,
    const CIMValue& myValue)
{
    if (format == OUTPUT_XML)
    {
        XmlWriter::printValueElement(myValue, cout);
    }
    else if (format == OUTPUT_MOF)
    {
        Buffer x;
        MofWriter::appendValueElement(x, myValue);
        mofFormat(cout, x.getData(), 4);
    }
    else
    {
        cerr << "Error: Format type error" << endl;
    }
}


// displaySummary for results of an operation. Display only if count
// ne zero.
// TODO: Separate the test from the count mechanism so that we can
// get a test and full display at the same time.
void _displayOperationSummary(Uint32 count,
    const String& description,
    const String item,
    Options& opts)
{
    if (count != 0)
    {
        cout << count << " " << description
            << " " << item << " returned.";

        if (opts.repeat > 0)
        {
            cout << " " << opts.repeat;
        }
        if(opts.time && opts.repeat > 0)
        {
            cout << " " << opts.saveElapsedTime;
        }
        cout << endl;
    }
}

void _testReturnCount(Options& opts,
    Uint32 count,
    const String& description,
    const String& item)
{

    if ((opts.count) != 29346 && (opts.count != count))
    {
        cerr << "Failed count test. Expected= " << opts.count
             << " Actual rcvd= " << count << endl;
        opts.termCondition = 1;
    }
}
/*
    The following are the public interfaces for the displays
*/

void CIMCLIOutput::displayOperationSummary(Options& opts,
    Uint32 count,
    const String& description,
    const String& item)
{
    _displayOperationSummary(count, description, item, opts);
    _testReturnCount(opts,
                     count,
                     description,
                     item);
}

void CIMCLIOutput::displayInstances(Options& opts,
const Array<CIMInstance>& instances)
{
    if (opts.summary)
    {
        String s = "instances of class";
        _displayOperationSummary(instances.size(), s,
                                 opts.className.getString(),opts);
    }
    else
    {
        if (instances.size() > 0 && opts.outputType == OUTPUT_TABLE)
        {
            tableDisplay(cout, instances);
            return;
        }

        // Output the returned instances
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            CIMInstance instance = instances[i];
            // Check Output Format to print results
            _outputFormatInstance(opts.outputType, instance);
        }
    }
}

void CIMCLIOutput::displayInstance(Options& opts,
    const CIMInstance& instance)
{
    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        if (opts.outputType == OUTPUT_TABLE)
        {
            Array<CIMInstance> instances;
            instances.append(instance);
            tableDisplay(cout, instances);
        }
        else
        {
            _outputFormatInstance(opts.outputType, instance);
        }
    }
}

void CIMCLIOutput::displayPath(Options& opts,
    const CIMObjectPath& path,
    const String& description)
{
    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        if (description.size() != 0)
        {
            cout << description << " ";
        }
        cout << path.toString() << endl;;
    }
}

void CIMCLIOutput::displayPaths(Options& opts,
    const Array<CIMObjectPath>& paths,
    const String& description)
{
    // TODO: Don't show anything if size = 0 and have the caller
    // setup the class somewhere external.
    if (opts.summary)
    {
        _displayOperationSummary(paths.size(), description,
                               opts.className.getString(), opts);
    }
    else
    {
        if (description.size() != 0 && paths.size() != 0)
        {
            cout << paths.size() << " " << description << endl;
        }
        //Output the list one per line for the moment.
        for (Uint32 i = 0; i < paths.size(); i++)
        {
            cout << paths[i].toString() << endl;
        }
    }
}

void CIMCLIOutput::displayClass(Options& opts,
    const CIMClass& cimClass)
{
    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        _outputFormatClass(opts.outputType, cimClass);
    }
}

void CIMCLIOutput::displayClassNames(Options& opts,
    const Array<CIMName>& classNames)
{
    if (opts.summary)
    {
      String s = "class names";
        _displayOperationSummary(classNames.size(), s,
             opts.className.getString(), opts);
    }
    else
    {
        //Output the list one per line for the moment.
        for (Uint32 i = 0; i < classNames.size(); i++)
        {
            cout << classNames[i].getString() << endl;
        }
    }
}

void CIMCLIOutput::displayProperty(Options& opts,
    const CIMProperty& property)
{
    _print(property, opts.outputType);
}

void CIMCLIOutput::displayValue(Options& opts,
    const CIMValue& value)
{
    _print(value, opts.outputType);
}

void CIMCLIOutput::displayObjects(Options& opts,
    const Array<CIMObject>& objects,
    const String& description)
{
    if (opts.summary)
    {
      String s = "instances of class";
        _displayOperationSummary(objects.size(), description,
                                  opts.className.getString(),opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
        {
            _outputFormatObject(opts.outputType, objects[i]);
        }

    }
}

void CIMCLIOutput::display(Options& opts,
    const String& desription)
{
    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        cout << desription << endl;
    }
}

void CIMCLIOutput::display(Options& opts,
    const char* desription)
{
    display(opts, String(desription));
}

void CIMCLIOutput::displayParamValues(Options& opts,
    const Array<CIMParamValue>& params)
{
    for (Uint32 i = 0; i < params.size() ; i++)
    {
        _outputFormatParamValue(opts.outputType, params[i]);
    }
}

void CIMCLIOutput::displayClasses(Options& opts,
    const Array<CIMClass>& classes)
{
    if (opts.summary)
    {
        String s = "classes";
        _displayOperationSummary(classes.size(), s,
                                 opts.className.getString(), opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < classes.size(); i++)
        {
            CIMClass myClass = classes[i];
            _outputFormatClass(opts.outputType, myClass);
        }
    }
}


void CIMCLIOutput::displayQualDecl(Options& opts,
    const CIMQualifierDecl& qualifierDecl)
{
    _outputFormatQualifierDecl(opts.outputType, qualifierDecl);
}


void CIMCLIOutput::displayQualDecls(Options& opts,
    const Array<CIMQualifierDecl> qualifierDecls)
{
    if (opts.summary)
    {
        cout << qualifierDecls.size() << " returned." << endl;
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < qualifierDecls.size(); i++)
        {
            CIMQualifierDecl myQualifierDecl = qualifierDecls[i];
            _outputFormatQualifierDecl(opts.outputType, myQualifierDecl);
        }
    }
}


PEGASUS_NAMESPACE_END
// END_OF_FILE

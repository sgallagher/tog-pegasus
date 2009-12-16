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

/*******************************************************************************

    Defines the display functions used by cimcli.  The are largely CIM entities
    and arrays of CIMEntities.  The output formats are controlled by
    data in the Options structure provider

*******************************************************************************/
#ifndef _CLI_OUTPUT_H
#define _CLI_OUTPUT_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/General/OptionManager.h>
#include <Clients/cimcli/Linkage.h>
#include <Pegasus/General/Stopwatch.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

class CIMCLIOutput
{
public:

    // Display an instance or an array of instances
    static void PEGASUS_CLI_LINKAGE displayInstance(Options& opts,
        const CIMInstance& instance);
    static void PEGASUS_CLI_LINKAGE displayInstances(Options& opts,
        const Array<CIMInstance>& instances);

    // Display either a single Class or array of classes.
    static void PEGASUS_CLI_LINKAGE displayClass(Options& opts,
        const CIMClass& cimClass);
    static void PEGASUS_CLI_LINKAGE displayClasses(Options& opts,
        const Array<CIMClass>& classes);

    // Display either an array of objects with optional description
    // text
    static void PEGASUS_CLI_LINKAGE displayObjects(Options& opts,
        const Array<CIMObject>& objects,
        const String& description);

    // Display a single path or an array of paths. There is an optional
    // parameter allowing adding a description that will be displayed
    // if paths exist.
    static void PEGASUS_CLI_LINKAGE displayPath(Options& opts,
        const CIMObjectPath& path,
        const String& description = String());
    static void PEGASUS_CLI_LINKAGE displayPaths(Options& opts,
        const Array<CIMObjectPath>& paths,
        const String& description = String());

    // display a single property
    static void PEGASUS_CLI_LINKAGE displayProperty(Options& opts,
        const CIMProperty& property);

    // Display a classname or array of classnames
    static void PEGASUS_CLI_LINKAGE displayClassName(Options& opts,
        const CIMName& className);
    static void PEGASUS_CLI_LINKAGE displayClassNames(Options& opts,
        const Array<CIMName>& classNames);

    static void PEGASUS_CLI_LINKAGE displayValue(Options& opts,
        const CIMValue& value);

    // display a single value
    static void PEGASUS_CLI_LINKAGE displayParamValues(Options& opts,
        const Array<CIMParamValue>& parms);

    static void PEGASUS_CLI_LINKAGE displayOperationSummary(Options& opts,
        Uint32 count,
        const String& description,
        const String& item);

    static void PEGASUS_CLI_LINKAGE display(Options& opts, const String& s);

    static void PEGASUS_CLI_LINKAGE display(Options& opts, const char* s);

    // Display a Qualifier Declaration or Array of Qualifier Declarations
    static void PEGASUS_CLI_LINKAGE displayQualDecl(Options& opts,
        const CIMQualifierDecl& qualifierDecl);
    static void PEGASUS_CLI_LINKAGE displayQualDecls(Options& opts,
        const Array<CIMQualifierDecl>
        qualifierDecls);

private:

};

PEGASUS_NAMESPACE_END

#endif

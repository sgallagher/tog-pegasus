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
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Common/Tracer.h>
#include "CIMCLIClient.h"
#include <Pegasus/Common/PegasusVersion.h>
PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

const String DEFAULT_NAMESPACE = "root/cimv2";

String _toString(Boolean x)
{
    return(x ? "true" : "false");
}
/** Select one item from an array of items presented to
    the user. This prints the list and requests user input for
    the response.
    @param selectList Array<String> list of items from which the
    user has to select one.  Each item should be a printable string.
    @param what String that defines for the output string what type
    of items the select is based on (ex: "Instance Names");
    @return Uint32 representing the item to be selected.
    TODO: Find a way to do a reject.

*/
Uint32 _selectStringItem(const Array<String>& selectList, const String& what)
{
    Uint32 rtn = 0;
    Uint32 listSize = selectList.size();

    for (Uint32 i = 0 ; i < listSize; i++)
        cout << i + 1 << ": " << selectList[i] << endl;

    while (rtn < 1 || rtn > listSize)
    {
        cout << "Select " << what 
             << " (1.." << listSize << ")? " << flush;

        // if input is not a valid integer, cin will be set to fail status.
        // and rtn will retain its previous value, so the loop could continue.
        cin >> rtn;
        
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(0x7fffffff, '\n');
        }
    }

    return (rtn - 1);
}

/** Allow user to select one instance name. Enumerates names given className and
    requests input of one index
    @param
    @param
    @param className CIMName for the class to enumerate.
    @param instancePath CIMObjectPath of instance found
    @return True if instance provided and the path is in instancePath.
    Else False and there is nothing in the instancePath
*/
Boolean _selectInstance(CIMClient& client, Options& opts, 
                        const CIMName& className,
    CIMObjectPath & instancePath)
{
    Array<CIMObjectPath> instanceNames =
        client.enumerateInstanceNames(opts.nameSpace,
                                      className);
    Array<String> list;
    for (Uint32 i = 0 ; i < instanceNames.size() ; i++)
        list.append(instanceNames[i].toString());

    if (list.size() == 0)
    {
        return(false);
    }
    Uint32 rtn = _selectStringItem(list, "an Instance");

    instancePath = instanceNames[rtn];

    return(true);
}


/** Use the interactive selection mechanism to get the instance if
    the input object is a class AND if the opts.interactive flag is
    set.  This function is used by the association functions because
    just the existence of the object as class is insufficient since
    these functions accept both class and instance input for processing.
    If the tests are passed this function calls the server to enumerate
    the instance names possible and displays them for the user to
    select one.
    @param client CIMClient context for the operation required
    @param opts the context structure for this operaiton
    @param name String of the object that is the target of the request.
    @param instancePath CIMObjectPath of instance selected if return
    is true.  Else, unchanged.
    @return Boolean True if an instance path is to be returned. If nothing
    is selected, returns False.
*/
Boolean _conditionalSelectInstance(CIMClient& client, Options& opts,
    CIMObjectPath & instancePath)
{
    // if class level and interactive set.
    if ((instancePath.getKeyBindings().size() == 0) && opts.interactive)
    {
        // Ask the user to select an instance

        return(_selectInstance(client, opts, CIMName(opts.objectName),
                    instancePath));
    }

    return(true);
}


// Character sequences used in help/usage output.
String buildPropertyListString(CIMPropertyList& pl)
{
    String rtn;
    Array<CIMName> pls = pl.getPropertyNameArray();
    if (pl.isNull())
        return("NULL");

    if (pl.size() == 0)
        return("EMPTY");

    for (Uint32 i = 0 ; i < pls.size() ; i++)
    {
        if (i != 0)
            rtn.append(", ");
        rtn.append(pls[i].getString());
    }
    return(rtn);
}

void printPropertyList(CIMPropertyList& pl)
{
    cout << buildPropertyListString(pl);
}
static const char * usage = "This command executes single CIM Operations.\n";

static const char* optionText = "Valid options for this command are : ";
static const char* commonOptions = "    -count, -d, -delay, -p, -l, -u, -o, -x,\
 -v, --sum, --timeout, -r, --t ";

struct CommandExampleWithOptionStruct
{
    const char* msgKey;  //Message key for globalization of example string
    const char* Example; //Example String
    const char* optionsKey; //Message key for option string
    const char* Options; //Option supported
};

CommandExampleWithOptionStruct examples[] = {
    {"Clients.cimcli.CIMCLIClient.NI_COMMAND_EXAMPLE",
    "cimcli ni -n test/TestProvider TEST_Person\n"
        "    -- Enumerate Instance Names of class\n",
    "Clients.cimcli.CIMCLIClient.NI_COMMAND_OPTIONS",
    "    -n, classname\n"},

    {"Clients.cimcli.CIMCLIClient.NIALL_COMMAND_EXAMPLE",
    "cimcli niall -n root/cimv2\n"
        "    -- Enumerate Instance Names of  all classes under\n"
        "       the namespace root/cimv2\n",
    "Clients.cimcli.CIMCLIClient.NIALL_COMMAND_OPTIONS",
    "    -n, -di, classname\n"},

    {"Clients.cimcli.CIMCLIClient.EI_COMMAND_EXAMPLE",
    "cimcli ei PG_ComputerSystem   -- Enumerate Instances of class\n",
    "Clients.cimcli.CIMCLIClient.EI_COMMAND_OPTIONS",
    "    -n, -di, -lo, -iq, -pl, classname, includeClassOrigin\n"},

    {"Clients.cimcli.CIMCLIClient.NC_COMMAND_EXAMPLE",
    "cimcli nc -- Enumerate class names from root/cimv2.\n",
    "Clients.cimcli.CIMCLIClient.NC_COMMAND_OPTIONS",
    "    -n, -di, classname\n"},

    {"Clients.cimcli.CIMCLIClient.EC_COMMAND_EXAMPLE",
    "cimcli ec -n root/cimv2\n"
        "    -- Enumerate classes from namespace root/cimv2.\n",
    "Clients.cimcli.CIMCLIClient.EC_COMMAND_OPTIONS",
    "    -n, -di, -lo, -iq, includeClassOrigin\n"},

    {"Clients.cimcli.CIMCLIClient.GC_COMMAND_EXAMPLE",
    "cimcli gc CIM_door -u guest -p guest\n"
        "    -- Get class user = guest and password = guest.\n",
    "Clients.cimcli.CIMCLIClient.GC_COMMAND_OPTIONS",
    "    -n, -lo, -iq, -pl, className, includeClassOrigin\n"},

    {"Clients.cimcli.CIMCLIClient.GI_COMMAND_EXAMPLE",
    "cimcli gi -n test/TestProvider TEST_Person\n"
        "    -- Get Instance of class\n",
    "Clients.cimcli.CIMCLIClient.GI_COMMAND_OPTIONS",
    "    -n, -lo, -iq, -pl, includeClassOrigin\n"},

    {"Clients.cimcli.CIMCLIClient.CI_COMMAND_EXAMPLE",
    "cimcli ci -n test/TestProvider TEST_Person Name=Michael\n"
        "    -- Create Instance of  class \n",
    "Clients.cimcli.CIMCLIClient.CI_COMMAND_OPTIONS",
    "    -n\n"},

    {"Clients.cimcli.CIMCLIClient.DI_COMMAND_EXAMPLE",
    "cimcli di -n test/TestProvider TEST_Person\n"
        "    -- Delete Instance of class interactively\n",
    "Clients.cimcli.CIMCLIClient.DI_COMMAND_OPTIONS",
    "    -n\n"},

    {"Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "Operation Not supported..\n",
    "Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "\n"},

    {"Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "Operation Not supported..\n",
    "Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "\n"},

    {"Clients.cimcli.CIMCLIClient.DC_COMMAND_EXAMPLE",
    "cimcli dc -n test/TestProvider TEST_Person\n"
        "    -- Deletes the Class when there are no instance and\n"
        "        sub-class for this class\n",
    "Clients.cimcli.CIMCLIClient.DC_COMMAND_OPTIONS",
    "    -n\n"},

    {"Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "Operation Not supported..\n",
    "Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "\n"},

    {"Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "Operation Not supported..\n",
    "Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "\n"},

    {"Clients.cimcli.CIMCLIClient.GQ_COMMAND_EXAMPLE",
    "cimcli gq Association\n"
        "    -- Get the qualifiers in mof output format\n",
    "Clients.cimcli.CIMCLIClient.GQ_COMMAND_OPTIONS",
    "    -n, qualifierName\n"},
        
    {"Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "Operation Not supported..\n",
    "Clients.cimcli.CIMCLIClient.OPERATION_NOT_SUPPORTED",
    "\n"},

    {"Clients.cimcli.CIMCLIClient.EQ_COMMAND_EXAMPLE",
    "cimcli eq -n test/TestProvider\n"
        "    -- Enumerate Qualifiers of namespace test/TestProvider\n",
    "Clients.cimcli.CIMCLIClient.EQ_COMMAND_OPTIONS",
    "    -n\n"},

    {"Clients.cimcli.CIMCLIClient.DQ_COMMAND_EXAMPLE",
    "cimcli dq -n test/TestProvider ASSOCIATION\n"
        "    -- Delete Qualifier Association in namespace\n"
        "        test/TestProvider\n",
    "Clients.cimcli.CIMCLIClient.DQ_COMMAND_OPTIONS",
    "    -n, qualifierName\n"},

    {"Clients.cimcli.CIMCLIClient.A_COMMAND_EXAMPLE",
    "cimcli a TST_Person.name=\\\"Mike\\\" -n test/TestProvider\n"
        "    -ac TST_Lineager"
        "\n or\n"
        "cimcli a TST_Person -n test/TestProvider -ac TST_Lineage\n",
    "Clients.cimcli.CIMCLIClient.A_COMMAND_OPTIONS",
    "    -n, -ac, -rc, -r, -rr, -iq, -pl, includeClassOrigin, -i\n"},

    {"Clients.cimcli.CIMCLIClient.AN_COMMAND_EXAMPLE",
    "cimcli an TST_Person.name=\\\"Mike\\\" -n test/TestProvider\n"
        "    -ac TST_Lineage"
        "\n or\n"
        "cimcli an TST_Person -n test/TestProvider -ac TST_Lineage\n",
    "Clients.cimcli.CIMCLIClient.AN_COMMAND_OPTIONS",
    "    -n, -ac, -rc, -r, -rr, -i\n"},

    {"Clients.cimcli.CIMCLIClient.R_COMMAND_EXAMPLE",
    "cimcli r TST_Person.name=\\\"Mike\\\" -n test/TestProvider\n"
        "    -rc TST_Lineage"
        "\n or\n"
        "cimcli r TST_Person -n test/TestProvider -rc TST_Lineage\n",
    "Clients.cimcli.CIMCLIClient.R_COMMAND_OPTIONS",
    "    -n, -rc, -r, -iq, -pl, includeClassOrigin, -i\n"},

    {"Clients.cimcli.CIMCLIClient.RN_COMMAND_EXAMPLE",
    "cimcli rn TST_Person.name=\\\"Mike\\\" -n test/TestProvider\n"
        "    -rc TST_Lineage"
        "\n or\n"
        "cimcli rn TST_Person -n test/TestProvider -rc TST_Lineage\n",
    "Clients.cimcli.CIMCLIClient.RN_COMMAND_OPTIONS",
    "    -n, -rc, -r, -i\n"},

    {"Clients.cimcli.CIMCLIClient.IM_COMMAND_EXAMPLE",
    "cimcli im Sample_MethodProviderClass.Name=\\\"mooo\\\" SayHello\n"
        "    -n root/SampleProvider -ip p1=fred\n",
    "Clients.cimcli.CIMCLIClient.IM_COMMAND_OPTIONS",
        "    -n, -ip\n"},

    {"Clients.cimcli.CIMCLIClient.XQ_COMMAND_EXAMPLE",
    "cimcli xq \"select * from CIM_ComputerSystem\"\n"
        "    -- This command will work when the code is compiled with\n"
        "        ExecQuery enabled\n",
    "Clients.cimcli.CIMCLIClient.XQ_COMMAND_OPTIONS",
    "    -n, queryLanguage, query\n"},

    {"Clients.cimcli.CIMCLIClient.NS_COMMAND_EXAMPLE",
    "cimcli ns  -- Enumerate all Namespaces in repository\n",
    "Clients.cimcli.CIMCLIClient.NO_OPTIONS_REQUIRED",
    " No options Required\n"},

    {"Clients.cimcli.CIMCLIClient.SON_COMMAND_EXAMPLE",
    "cimcli son  -- Switch On's the statistical information on CIMServer\n",
    "Clients.cimcli.CIMCLIClient.NO_OPTIONS_REQUIRED",
    " No options Required\n"},

    {"Clients.cimcli.CIMCLIClient.SOFF_COMMAND_EXAMPLE",
    "cimcli soff  -- Switch OFF's the statistical information on CIMServer\n",
    "Clients.cimcli.CIMCLIClient.NO_OPTIONS_REQUIRED",
    " No options Required\n"},

    {"Clients.cimcli.CIMCLIClient.?_COMMAND_EXAMPLE",
    "cimcli ?  -- Displays help command\n",
    "Clients.cimcli.CIMCLIClient.NO_OPTIONS_REQUIRED",
    "    No options Required"}
};

void showExamples()
{
    Uint32 numExamples = sizeof(examples) / sizeof(examples[0]);
    cout << 
        loadMessage(
            "Clients.cimcli.CIMCLIClient.EXAMPLES_STRING",
            "Examples : ") 
         << endl;

    for (Uint32 i=0; i < numExamples; i++)
    {
        cout << loadMessage(examples[i].msgKey, examples[i].Example) << endl;
    }
}

void _displaySummary(Uint32 count, String& description, String item,
                     Options& opts)
{
        cout << count << " " << description
            << " " << item << " returned. ";
        if (opts.repeat > 0)
            cout << opts.repeat;
        if(opts.time && opts.repeat)
            cout << " " << opts.saveElapsedTime;
        cout << endl;
        if ((opts.count) != 29346 && (opts.count != count))
        {
            cout << "Failed count test. Expected= " << opts.count 
                 << " Actual rcvd= " << count << endl;
            opts.termCondition = 1;
        }
}

/**** NOT TESTED
String _nextToken(String& input, Uint32 start, Char16 separator)
{
    String rtn;
    Uint32 end;
    end = input.find(input.find(start, separator));
    if (end == PEG_NOT_FOUND)
    {
        start = PEG_NOT_FOUND
        rtn = input.subString(start,(input.size() - start);
    }
    else
    {
        rtn = input.subString(start,end);
        start = end + 1;
    }
    return(rtn);
}
*/
/** tokenize an input string into an array of Strings,
 * separating the tokens at the separator character
 * provided
 * @param input String
 * @param separator character
 * @returns Array of separated strings
 * */
Array<String> _tokenize(const String& input, const Char16 separator)
{
    Array<String> tokens;
    if (input.size() != 0)
    {
        Uint32 start = 0;
        Uint32 length = 0;
        Uint32 end = 0;
        while ((end = input.find(start, separator)) != PEG_NOT_FOUND)
        {
            length = end - start;
            tokens.append(input.subString(start, length));
            start += (length + 1);
        }
        //Replaced < with <= to consider input param like A="" as valid param.
        //key in this param is 'A'and value is NULL.
        //It also takes care of A= param.
        if(start <= input.size())
        {
            tokens.append(input.subString(start));
        }
    }
    return tokens;
}

Boolean _tokenPair(const String& input, String& key, String& value)
{

    Array<String> pair = _tokenize(input, '=');
    if (pair.size() < 2)
    {
        cout << "Input Parameter error. Expected name=value. Received  " 
             << input << endl;
        return(false);
    }
    // If there is more than 1 "=" it is part of the reference and we
    // rebuild the reference.
    if (pair.size() > 2)
    {
        for (Uint32 i = 2 ; i < pair.size() ; i++)
        {
            pair[1].append("=");
            pair[1].append(pair[i]);
        }
    }
    key = pair[0];
    value = pair[1];
    return(true);
}


void _nextParamToken(String& input, String& token)
{
    unsigned int end;
    // Check for Character Literal
    if (input.find('\'') == 0)
    {
        input.remove(0,1);  // remove quote
        if ((end = input.find("\'")) != PEG_NOT_FOUND)
        {
            token = input.subString(0, end);
            input.remove(0, end + 1);   // +1 to delete final quote char
        }
        else
        {
            printf(" ERROR -- no matching quote!");
        }
        return;
    }
    // Check for String Literal
    if (input.find('\"') == 0)
    {
        input.remove(0,1);  // remove quote
        if ((end = input.find("\"")) != PEG_NOT_FOUND)
        {
            token = input.subString(0, end);
            input.remove(0, end + 1);   // +1 to delete final quote char
        }
        else
        {
            printf("ERROR -- no matching quote!");
        }
        return;
    }
    // Take line up to separator (, or }) as token
    if ((end = input.find(',')) != PEG_NOT_FOUND)
    {
        token = input.subString(0, end);
        input.remove(0, end+1); // +1 to capture the comma
        return;
    }
    if ((end = input.find('}')) != PEG_NOT_FOUND)
    {
        token = input.subString(0, end);
        input.remove(0, end+1); // +1 to capture the brace
        return;
    }

    if((end = input.find(']')) != PEG_NOT_FOUND)
    {
        token = input.subString(0, end);
        input.remove(0, end+1); // +1 to capture the bracket
        return;
    }
    // Fall through.. take entire input as token
    token = input;
    input = "";
    return;
}

CIMParamValue _createMethodParamValue(const String& input, const Options& opts)
{
    String key;
    String value;
    if (!_tokenPair(input, key, value))
    {
        cout <<"Error in param parsing with input " << input << endl;
        exit(1);
    }

    if (opts.verboseTest)
    {
        cout << "Name = " << key << ", Value= " << value << endl;
    }
    // ATTN: KS 20030423 P2 This is incomplete since it only allows us
    // to do string input.
    // We don't include the typing information.
    //Array<String> valuePair = _tokenize(pair[1], ':');
    //if (validType(valuePair[0] >= 0)
    //{
    //
    //}

    String tmp = value;
    if (value.find('{') == 0)
    {
        // assume brace at first character position indictates an
        // array value
        tmp.remove(0,1);  // remove open brace
        Array<String> arr;
        while (tmp.size() != 0)
        {
            String token;
            _nextParamToken(tmp, token);
            arr.append(token);
            // Now remove token separators (comma, brace or whitespace)
            while ((tmp.size() > 0) && 
                   ((tmp.find(",") == 0) || (tmp.find("}") == 0) || 
                    (tmp.find(" ") == 0)))
                tmp.remove(0,1);
        }
        CIMValue v(arr);
        CIMParamValue pv(key, v);
        return pv;
    }

    // Check for References starting with '[' and ending with ']'
    if(tmp.find('[') == 0)
    {
        if(tmp.find(']') == tmp.size()-1)
        {
            Array<CIMKeyBinding> keys;
            Array<String> arr;
            String className;
            tmp.remove(0,1);  // remove open bracket

            // Iterate over the input param to extract class name, 
            // key names and values.
            while(tmp.size() != 0)
            {
                String token, identifier, refKey;
                _nextParamToken(tmp, token);
                Uint32 dotIndex = 0, equalIndex = 0;

                // Get the class name and key1/value1 from class.key = value
                if (((dotIndex = token.find('.')) != PEG_NOT_FOUND)  &&
                     (((equalIndex = token.find('=')) != PEG_NOT_FOUND) && 
                       dotIndex < equalIndex-1))
                {
                    //extract class name, key1 and value1
                    className = token.subString(0, dotIndex);
                    identifier = token.subString(dotIndex+1, 
                                                 equalIndex-1-dotIndex);
                    refKey = token.subString(equalIndex+1, token.size());
                    keys.append(CIMKeyBinding(identifier, refKey,
                                              CIMKeyBinding::STRING));
                }

                // Get the simple keyX = valueX (where X>1)
                else if((equalIndex = token.find('=')) != PEG_NOT_FOUND)
                {
                    identifier = token.subString(0, equalIndex);
                    refKey = token.subString(equalIndex+1, token.size());
                    keys.append(CIMKeyBinding(identifier, refKey,
                                              CIMKeyBinding::STRING));
                }
                else
                {
                    cout << " Error in the reference param this could be"
                                " a string param" << endl;
                    exit(1);
                }
                // Now remove token separators (comma, bracket or whitespace)
                while((tmp.size() > 0) && 
                      ((tmp.find(",") == 0) || 
                       (tmp.find("]") == 0) || 
                       (tmp.find(" ") == 0)))
                {
                    tmp.remove(0,1);
                }
            }
            // Reference param specified is valid. 
            // Make CIM Object Path from the token.
            CIMName cimclassName(className);
            CIMObjectPath cop(String::EMPTY,CIMNamespaceName(opts.nameSpace),
                              cimclassName,keys);
            CIMValue v(cop);
            CIMParamValue pv(key, v, false);
            return pv;
        }
        else
        {
            cout <<"Treat this as String param " << input << endl;
        }
    }

    // Fallthrough...
    CIMValue v(value);
    CIMParamValue pv(key, v, false);
    return pv;
}

void outputFormatInstance(const OutputType format, CIMInstance& instance)
{
    cout << "path= " << instance.getPath().toString() << endl;
    if (format == OUTPUT_XML)
        XmlWriter::printInstanceElement(instance, cout);
    else if (format == OUTPUT_MOF)
    {
        // Reset the propagated flag to assure that these entities
        // are all shown in the MOF output.
        for (Uint32 i = 0 ; i < instance.getPropertyCount() ; i++)
        {
            CIMProperty p = instance.getProperty(i);
            p.setPropagated(false);
        }

        Buffer x;
        MofWriter::appendInstanceElement(x, instance);
        mofFormat(cout, x.getData(), 4);
    }
}
void outputFormatParamValue(const OutputType format, const CIMParamValue& pv)
{
    if (format == OUTPUT_XML)
        XmlWriter::printParamValueElement(pv, cout);

    else if (format == OUTPUT_MOF)
    {
        if (!pv.isUninitialized())
        {
           CIMValue v =  pv.getValue();
           CIMType type = v.getType();
           if (pv.isTyped())
               cout << cimTypeToString (type) << " ";
           else
               cout << "UnTyped ";

           cout << pv.getParameterName() << "="
                << v.toString() << endl;
        }
        else
            cout << "ParamValue not initialized" << endl;
    }
    else
        cout << "Error, Format Definition Error" << endl;
}

void outputFormatClass(const OutputType format, CIMClass& myClass)
{
    if (format == OUTPUT_XML)
        XmlWriter::printClassElement(myClass, cout);
    else if (format == OUTPUT_MOF)
    {
        // Reset the propagated flag to assure that these entities
        // are all shown in the MOF output.
        for (Uint32 i = 0 ; i < myClass.getPropertyCount() ; i++)
        {
            CIMProperty p = myClass.getProperty(i);
            p.setPropagated(false);
        }
        for (Uint32 i = 0 ; i < myClass.getMethodCount() ; i++)
        {
            CIMMethod m = myClass.getMethod(i);
            m.setPropagated(false);
        }
        Buffer x;
        MofWriter::appendClassElement(x, myClass);
        mofFormat(cout, x.getData(), 4);
    }
    else
        cout << "Error, Format Definition Error" << endl;
}

void outputFormatObject(const OutputType format, const CIMObject& myObject)
{

    if (myObject.isClass())
    {
        CIMClass c(myObject);
        outputFormatClass(format, c);
    }
    else if (myObject.isInstance())
    {
        CIMInstance i(myObject);
        outputFormatInstance(format, i);
    }
    else
        cout << "Error, Object is neither class or instance" << endl;
}

void outputFormatQualifierDecl(const OutputType format, 
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
        cout << "Format type error" << endl;
    }
}

void outputFormatCIMValue(const OutputType format, const CIMValue& myValue)
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
        cout << " Format type error" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//                                                               //
//     The following section  defines each action function       //
//     ex. getInstance. Parameters are defined in the            //
//     opts structure.  There are not exception catches.         //
//     exception handling is in the main path                    //
///////////////////////////////////////////////////////////////////

/* This command searches the entire namespace and displays names of
   all instances.
   It is in effect enumerate classes followed by enumerate instances.
   The user may either provide a starting class or not, in which case
   it searches the complete namespace, not simply the defined class.
*/
int enumerateAllInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className.getString()
            << ", deepInheritance = " << _toString(opts.deepInheritance)
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    CIMName myClassName = CIMName();
    /****if (opts.className != "")
    {
        myClassName = opts.className;
    }*/

    Array<CIMName> classNames;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance);

    for (Uint32 iClass = 0; iClass < classNames.size(); iClass++)
    {
        if (opts.verboseTest)
        {
            cout << "EnumerateInstanceNames "
                << "Namespace = " << opts.nameSpace
                << ", Class = " << classNames[iClass].getString()
                << endl;
        }
        Array<CIMObjectPath> instanceNames =
        client.enumerateInstanceNames(opts.nameSpace, classNames[iClass]);
        if (opts.summary)
        {
      String s = "instance names of class";
      _displaySummary(instanceNames.size(), s, opts.className.getString(),opts);
        }
        else
        {
            //simply output the list one per line for the moment.
            for (Uint32 i = 0; i < instanceNames.size(); i++)
                        cout << instanceNames[i].toString() << endl;
        }
    }

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int enumerateInstanceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstanceNames "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className.getString()
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObjectPath> instanceNames =
        client.enumerateInstanceNames(opts.nameSpace,
                                      opts.className);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "instances names of class";
      _displaySummary(instanceNames.size(), s, opts.className.getString(),opts);
    }
    else
    {
        //Output the list one per line for the moment.
        for (Uint32 i = 0; i < instanceNames.size(); i++)
                    cout << instanceNames[i].toString() << endl;
    }

    return(0);
}

int enumerateInstances(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateInstances "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className.getString()
            << ", deepInheritance = " << _toString(opts.deepInheritance)
            << ", localOnly = " << _toString(opts.localOnly)
            << ", includeQualifiers = " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin = " << _toString(opts.includeClassOrigin)
            << ", PropertyList = " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    Array<CIMInstance> instances;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    instances = client.enumerateInstances( opts.nameSpace,
                                           opts.className,
                                           opts.deepInheritance,
                                           opts.localOnly,
                                           opts.includeQualifiers,
                                           opts.includeClassOrigin,
                                           opts.propertyList );

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "instances of class";
        _displaySummary(instances.size(), s, opts.className.getString(),opts);
    }
    else
    {
        if (instances.size() > 0 && opts.outputType == OUTPUT_TABLE)
        {
            tableFormat(cout, instances);
            return(0);
        }

        // Output the returned instances
        for (Uint32 i = 0; i < instances.size(); i++)
        {
            CIMInstance instance = instances[i];
            // Check Output Format to print results
            outputFormatInstance(opts.outputType, instance);
        }
    }

    return(0);
}

int executeQuery(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "ExecQuery "
            << "Namespace = " << opts.nameSpace
            << ", queryLanguage = " << opts.queryLanguage
            << ", query = " << opts.query
            << endl;
    }

    Array<CIMObject> objects;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    objects = client.execQuery( opts.nameSpace,
                                opts.queryLanguage,
                                opts.query );

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "instances of class";
        _displaySummary(objects.size(), s, opts.className.getString(),opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
            outputFormatObject(opts.outputType, objects[i]);

    }

    return(0);
}

int deleteInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteInstance "
            << "Namespace = " << opts.nameSpace
            << ", Object = " << opts.objectName
            << endl;
    }
    // if request is class only, do this interactively
    // Need to get this into objectpath format before doing the call.
    CIMObjectPath thisObject(opts.objectName);
    if ((thisObject.getKeyBindings().size() == 0) ? true : false)
    {
        // get the instance to delete
        if(!_selectInstance(client, opts, CIMName(opts.objectName), thisObject))
            return(0);
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.deleteInstance(opts.nameSpace,
                          thisObject);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}
/*
NAMESPACE = 1
Class = 2
OBJECT = Object*2
OBJECTNAME = OBJECT*2
INSTANCENAME = OBJECTNAME * 2
METHODNAME = INSTANCENAME * 2
PROPERTYNAME = 2
PROPERTYVALUE=2
INPARAMS = 2
DEEPINHERITANCE = 2
LOCALONLY = 2
ASSOCCLASS = 4
RESULTCLASS = 8
ROLE = 16
RESULTROLE = 32
InCLUDEQUALIFIERS = 64
INCLUDECLASSORIGIN = 128
PROPDERTYLIST = 256
*/

int getInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getInstance "
            << "Namespace = " << opts.nameSpace
            << ", Instance = " << opts.objectName
            << ", localOnly = " << _toString(opts.localOnly)
            << ", includeQualifiers = " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin = " << _toString(opts.includeClassOrigin)
            << ", PropertyList = " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    CIMObjectPath thisObject(opts.objectName);

    if ((thisObject.getKeyBindings().size() == 0) ? true : false)
    {
        // get the instance to delete
        if(!_selectInstance(client, opts, CIMName(opts.objectName),thisObject))
            return(0);
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    CIMInstance cimInstance = client.getInstance(opts.nameSpace,
                                                 thisObject,
                                                 opts.localOnly,
                                                 opts.includeQualifiers,
                                                 opts.includeClassOrigin,
                                                 opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // Check Output Format to print results
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
            instances.append(cimInstance);
            tableFormat(cout, instances);
        }
        else
        {
            outputFormatInstance(opts.outputType, cimInstance);
        }
    }

    return(0);
}

/****
    CIMObjectPath createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance
    );
 ***/
int createInstance(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "createInstance "
            << "Namespace = " << opts.nameSpace
            << ", ClassName = " << opts.className.getString()
            << endl;
    }
    // get the class. Exceptions including class_not_found are automatic
    CIMClass thisClass =
        client.getClass(opts.nameSpace, opts.className,
                        false,true,true,CIMPropertyList());

    // Tokenize the parameter pairs
    //Array<keyValuePair> inputs;

    Array<CIMName> propertyNameList;
    Array<String> propertyValueList;

    // ATTN: Need to account for returning key without value here.
    if (opts.extraParams != 0)
    {
        /* Here loop starts from 1, since the Class Name is coming as 
         * first parameter and we want only the property name and value here
        */
        for (Uint32 i = 1 ; i < opts.extraParams.size() ; i++)
        {
            String key;
            String value;
            _tokenPair(opts.extraParams[i], key, value);
            propertyNameList.append(CIMName(key));
            propertyValueList.append(value);
            if (thisClass.findProperty(CIMName(key)) == PEG_NOT_FOUND)
                cout << "Warning property Name not in class: " 
                     << opts.extraParams[i] << endl;
        }

        if (opts.verboseTest)
        {
            // This loop gives all the property names and property values 
            // of the instance
            for (Uint32 i=0; i < propertyNameList.size(); i++)
            {
                cout << "Property: " << propertyNameList[i].getString()
                     << " value: " << propertyValueList[i]
                     << endl;
            }
        }
    }

    CIMPropertyList myPropertyList(propertyNameList);
    // create the instance with the defined properties
    CIMInstance newInstance = thisClass.buildInstance(true, true,
                                                      myPropertyList);

    // Set all the property Values to the instance
    for (Uint32 i=0; i < propertyValueList.size(); i++)
    {
        newInstance.getProperty(i).setValue(CIMValue( propertyValueList[i]));
    }
    // Now add the parameters from the input. Array.
    //Note that we do NO checking.  Each input parameter is a simple
    //name=value.
    // At this point we also treat them all as strings since we have not
    // defined a means to handle typing.

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    CIMObjectPath rtndPath = client.createInstance(opts.nameSpace,
                                                 newInstance);

    // Need to put values into the parameters.

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // Check Output Format to print results
    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        cout << rtndPath.toString() << endl;;
    }

    return(0);
}

int enumerateClassNames(CIMClient& client, Options& opts)
{

    if (opts.verboseTest)
    {
        cout << "EnumerateClasseNames "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className.getString()
            << ", deepInheritance= " << (opts.deepInheritance? "true" : "false")
            << endl;
    }
    Array<CIMName> classNames;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    classNames = client.enumerateClassNames(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "class names";
        _displaySummary(classNames.size(), s,
             opts.className.getString(),opts);
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < classNames.size(); i++)
                cout << classNames[i].getString() << endl;
    }

    return(0);
}

int enumerateClasses(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateClasses "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className.getString()
            << ", deepInheritance= " << _toString(opts.deepInheritance)
            << ", localOnly= " << _toString(opts.localOnly)
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << endl;
    }
    // Added to allow "" string input to represent NULL CIMName.
    /*
    CIMName myClassName = CIMName();
    if (opts.className != "")
    {
        myClassName = opts.className;
    }
    */
    Array<CIMClass> classes;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    classes = client.enumerateClasses(opts.nameSpace,
                                        opts.className,
                                        opts.deepInheritance,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
        String s = "classes";
        _displaySummary(classes.size(), s, opts.className.getString(),opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < classes.size(); i++)
        {
            CIMClass myClass = classes[i];
            outputFormatClass(opts.outputType, myClass);
        }
    }

    return(0);
}

int deleteClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteClasses "
            << "Namespace = " << opts.nameSpace
            << ", Class = " << opts.className.getString()
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.deleteClass(opts.nameSpace, opts.className);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int getClass(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getClass "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className.getString()
            << ", deepInheritance= " << _toString(opts.deepInheritance)
            << ", localOnly= " << _toString(opts.localOnly)
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << ", PropertyList= " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    CIMClass cimClass = client.getClass(opts.nameSpace,
                                        opts.className,
                                        opts.localOnly,
                                        opts.includeQualifiers,
                                        opts.includeClassOrigin,
                                        opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        outputFormatClass(opts.outputType, cimClass);
    }
    return(0);
}

int getProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "getProperty "
            << "Namespace= " << opts.nameSpace
            << ", Instance= " << opts.instanceName.toString()
            << ", propertyName= " << opts.propertyName
            << endl;
    }

    CIMValue cimValue;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    cimValue = client.getProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName);
    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
        if (opts.time)
        {
            cout << opts.saveElapsedTime << endl;
        }
    }
    else
    {
        cout << opts.propertyName << " = " << cimValue.toString() << endl;
    }
    
    return(0);
}

int setProperty(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setProperty "
            << "Namespace= " << opts.nameSpace
            << ", Instance= " << opts.instanceName.toString()
            << ", propertyName= " << opts.propertyName
            << ", newValue= " << opts.newValue.toString()
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.setProperty( opts.nameSpace,
                                   opts.instanceName,
                                   opts.propertyName,
                                   opts.newValue);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int getQualifier(CIMClient& client, Options& opts)
{
    cout << "entering getQualifier " << endl;
    //if (opts.verboseTest)
    {
        cout << "getQualifier "
            << "Namespace= " << opts.nameSpace
            << ", Qualifier= " << opts.qualifierName
            << endl;
    }

    CIMQualifierDecl cimQualifierDecl;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    cimQualifierDecl = client.getQualifier( opts.nameSpace,
                                   opts.qualifierName);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    // display new qualifier

    outputFormatQualifierDecl(opts.outputType, cimQualifierDecl);

    return(0);
}

int setQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "setQualifiers "
            << "Namespace= " << opts.nameSpace
            // KS add the qualifier decl here.
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.setQualifier( opts.nameSpace,
                         opts.qualifierDeclaration);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}

int deleteQualifier(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "deleteQualifiers "
            << "Namespace= " << opts.nameSpace
            << " Qualifier= " << opts.qualifierName
            << endl;
    }

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    client.deleteQualifier( opts.nameSpace,
                            opts.qualifierName);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    return(0);
}
int enumerateQualifiers(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "enumerateQualifiers "
            << "Namespace= " << opts.nameSpace
            << endl;
    }

    Array<CIMQualifierDecl> qualifierDecls;

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    qualifierDecls = client.enumerateQualifiers( opts.nameSpace);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
        cout << qualifierDecls.size() << " returned." << endl;
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < qualifierDecls.size(); i++)
        {
            CIMQualifierDecl myQualifierDecl = qualifierDecls[i];
            outputFormatQualifierDecl(opts.outputType, myQualifierDecl);
        }
    }

    return(0);
}

/*Array<CIMObjectPath> referenceNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY
*/
int referenceNames(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "ReferenceNames "
            << "Namespace= " << opts.nameSpace
            << ", ObjectPath= " << opts.objectName
            << ", resultClass= " << opts.resultClass.getString()
            << ", role= " << opts.role
            << endl;
    }
    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if (!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObjectPath> referenceNames =
        client.referenceNames( opts.nameSpace,
                               thisObjectPath,
                               opts.resultClass,
                               opts.role);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    /*
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass = CIMName(),
    const String& role = String::EMPTY */

    if (opts.summary)
    {
      String s = "referenceNames";
        _displaySummary(referenceNames.size(),s,
             opts.objectName,opts);
    }
    else
    {
        //simply output the list one per line for the moment.
        for (Uint32 i = 0; i < referenceNames.size(); i++)
                    cout << referenceNames[i].toString() << endl;
    }

    return(0);
}


/****
     Array<CIMObject> references(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    );
    */
int references(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "References "
            << "Namespace= " << opts.nameSpace
            << ", Object= " << opts.objectName
            << ", resultClass= " << opts.resultClass.getString()
            << ", role= " << opts.role
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << ", CIMPropertyList= "  
            << buildPropertyListString(opts.propertyList)
            << endl;
    }

    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if (!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObject> objects =
        client.references(  opts.nameSpace,
                            thisObjectPath,
                            opts.resultClass,
                            opts.role,
                            opts.includeQualifiers,
                            opts.includeClassOrigin,
                            opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "references";
        _displaySummary(objects.size(), s, opts.objectName,opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
            outputFormatObject(opts.outputType, objects[i]);
    }

    return(0);
}

/*
    Array<CIMObjectPath> associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass = CIMName(),
    const CIMName& resultClass = CIMName(),
    const String& role = String::EMPTY,
    const String& resultRole = String::EMPTY
    );

*/
int associatorNames(CIMClient& client, Options& opts)
{

    if (opts.verboseTest)
    {
        cout << "associatorNames "
            << "Namespace= " << opts.nameSpace
            << ", Object= " << opts.objectName
            << ", assocClass= " << opts.assocClass.getString()
            << ", resultClass= " << opts.resultClass.getString()
            << ", role= " << opts.role
            << ", resultRole= " << opts.resultRole
            << endl;
    }

    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if(!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObjectPath> associatorNames =
    client.associatorNames( opts.nameSpace,
                            thisObjectPath,
                            opts.assocClass,
                            opts.resultClass,
                            opts.role,
                            opts.resultRole);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    /*
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass = CIMName(),
    const String& role = String::EMPTY */

    if (opts.summary)
    {
      String s = "associator names";
        _displaySummary(associatorNames.size(), s,
             opts.objectName,opts);
    }
    else
    {
        // Output the list one per line for the moment.
        for (Uint32 i = 0; i < associatorNames.size(); i++)
                    cout << associatorNames[i].toString() << endl;
    }

    return(0);
}


/****
    Array<CIMObject> associators(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass = CIMName(),
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList()
    );
 */
int associators(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "Associators "
            << "Namespace= " << opts.nameSpace
            << ", Object= " << opts.objectName
            << ", assocClass= " << opts.assocClass.getString()
            << ", resultClass= " << opts.resultClass.getString()
            << ", role= " << opts.role
            << ", resultRole= " << opts.resultRole
            << ", includeQualifiers= " << _toString(opts.includeQualifiers)
            << ", includeClassOrigin= " << _toString(opts.includeClassOrigin)
            << ", propertyList= " << buildPropertyListString(opts.propertyList)
            << endl;
    }

    // do conditional select of instance if params properly set.
    CIMObjectPath thisObjectPath(opts.objectName);
    if(!_conditionalSelectInstance(client, opts, thisObjectPath))
        return(0);

    if (opts.time)
    {
        opts.elapsedTime.reset();
        opts.elapsedTime.start();
    }

    Array<CIMObject> objects =
    client.associators( opts.nameSpace,
                        thisObjectPath,
                        opts.assocClass,
                        opts.resultClass,
                        opts.role,
                        opts.resultRole,
                        opts.includeQualifiers,
                        opts.includeClassOrigin,
                        opts.propertyList);

    if (opts.time)
    {
        opts.elapsedTime.stop();

        opts.saveElapsedTime = opts.elapsedTime.getElapsed();
    }

    if (opts.summary)
    {
      String s = "associators";
        _displaySummary(objects.size(), s, opts.objectName,opts);
    }
    else
    {
        // Output the returned instances
        for (Uint32 i = 0; i < objects.size(); i++)
            outputFormatObject(opts.outputType, objects[i]);
    }

    return(0);
}

/*
    CIMValue invokeMethod(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& methodName,
        const Array<CIMParamValue>& inParameters,
        Array<CIMParamValue>& outParameters
*/

 int invokeMethod(CIMClient& client, Options& opts)
 {
     {
         // Display the parameter set if verbose requested.
         if (opts.verboseTest)
         {
             cout << "invokeMethod"
                 << " Namespace= " << opts.nameSpace
                 << ", ObjectName= " << opts.objectName
                 << ", methodName= " << opts.methodName.getString()
                 << ", inParams Count= " << opts.inParams.size()
                 << endl;
             for (Uint32 i=0; i< opts.inParams.size(); i++)
                 outputFormatParamValue(opts.outputType, opts.inParams[i]);
        }

         // Create array for output parameters
        CIMValue retValue;
        Array<CIMParamValue> outParams;

        if (opts.time)
        {
            opts.elapsedTime.reset();
            opts.elapsedTime.start();
        }

        // Call invoke method with the parameters
        retValue = client.invokeMethod(opts.nameSpace, opts.objectName,
            opts.methodName, opts.inParams, outParams);

        if (opts.time)
        {
            opts.elapsedTime.stop();

            opts.saveElapsedTime = opts.elapsedTime.getElapsed();
        }

        // Display the return value CIMValue
        cout << "Return Value= ";
        if (opts.outputType == OUTPUT_XML)
            XmlWriter::printValueElement(retValue, cout);
        else
            cout << retValue.toString() << endl;

        // Display any outparms
        for (Uint32 i = 0; i < outParams.size() ; i++)
            outputFormatParamValue(opts.outputType, outParams[i]);

     }

     return(0);
 }

 /* Enumerate the Namespaces.  This function is based on using the 
    __Namespace class and either returns all namespaces or simply the ones
    starting at the namespace input as the namespace variable.
    It assumes that the input classname is __Namespace.
 */
int enumerateNamespaces_Namespace(CIMClient& client, Options& opts)
{
    if (opts.verboseTest)
    {
        cout << "EnumerateNamespaces "
            << "Namespace= " << opts.nameSpace
            << ", Class= " << opts.className.getString()
            << endl;
    }

    Boolean usingPegasus = true;
    Array<CIMInstance> instances;

    try
    {
        instances = client.enumerateInstances(
            (CIMNamespaceName)(opts.nameSpace),opts.className);
    }
    catch(CIMException &)
    {
        /*if an exception was caught here then we assume we are not useing
        the open pegasus CIMOM. There for we should only check the
        __namespaces class. (Which may only retrun a subset of all namspaces
        */
        usingPegasus = false;
        opts.className = CIMName("__namespace");
        opts.nameSpace = PEGASUS_NAMESPACENAME_INTEROP.getString();
    }

    if (usingPegasus)
    {
        for (Uint32 i = 0 ; i < instances.size(); i++)
        {
            Uint32 pos;
            // if we find the property and it is a string, use it.
            if ((pos = instances[i].findProperty("name")) != PEG_NOT_FOUND)
            {
                CIMValue value;
                String namespaceComponent;
                value = instances[i].getProperty(pos).getValue();
                if (value.getType() == CIMTYPE_STRING)
                {
                    value.get(namespaceComponent);
                    cout << namespaceComponent << endl;
                }
            }
        }

    }
    else
    {
        Array<CIMNamespaceName> namespaceNames;

        // Build the namespaces incrementally starting at the root
        // ATTN: 20030319 KS today we start with the "root" directory but
        // this is wrong. We should be
        // starting with null (no directory) but today we get an xml error
        // return in Pegasus
        // returned for this call. Note that the specification requires
        // that the root namespace be used
        // when __namespace is defined but does not require that it be
        // the root for allnamespaces. That
        // is a hole is the spec, not in our code.
        namespaceNames.append(opts.nameSpace);
        Uint32 start = 0;
        Uint32 end = namespaceNames.size();


        if (opts.time)
        {
            opts.elapsedTime.reset();
            opts.elapsedTime.start();
        }

        do
        {
            // for all new elements in the output array
            for (Uint32 range = start; range < end; range ++)
            {
                // Get the next increment in naming for all a name element
                // in the array
                instances = client.enumerateInstances(
                    namespaceNames[range],opts.className);
                for (Uint32 i = 0 ; i < instances.size(); i++)
                {
                    Uint32 pos;
                    // if we find the property and it is a string, use it.
                    if ((pos = instances[i].findProperty("name")) 
                            != PEG_NOT_FOUND)
                    {
                        CIMValue value;
                        String namespaceComponent;
                        value = instances[i].getProperty(pos).getValue();
                        if (value.getType() == CIMTYPE_STRING)
                        {
                            value.get(namespaceComponent);

                                String ns = namespaceNames[range].getString();
                                ns.append("/");
                                ns.append(namespaceComponent);
                                namespaceNames.append(ns);
                        }
                    }
                }
                start = end;
                end = namespaceNames.size();
            }
        }
        while (start != end);


        // Validate that all of the returned entities are really namespaces.
        // It is legal for us to have an name component that is really not a
        // namespace (ex. root/fred/john is a namespace  but root/fred is not.
        // There is no clearly defined test for this so we will simply try to
        // get something, in this
        // case a wellknown assoication
          Array<CIMNamespaceName> returnNamespaces;

        for (Uint32 i = 0 ; i < namespaceNames.size() ; i++)
        {
            try
            {
                CIMQualifierDecl cimQualifierDecl;
                cimQualifierDecl = client.getQualifier(namespaceNames[i],
                                               "Association");

                returnNamespaces.append(namespaceNames[i]);
            }
            catch(CIMException& e)
            {
                if (e.getCode() != CIM_ERR_INVALID_NAMESPACE)
                    returnNamespaces.append(namespaceNames[i]);
            }
        }

        if (opts.time)
        {
            opts.elapsedTime.stop();

            opts.saveElapsedTime = opts.elapsedTime.getElapsed();
        }

        if (opts.summary)
        {
            cout << returnNamespaces.size() << " namespaces " << " returned." 
                << endl;
        }
        else
        {
            for( Uint32 cnt = 0 ; cnt < returnNamespaces.size(); cnt++ )
            {
                cout << returnNamespaces[cnt].getString() << endl;;
            }
        }
    }

    return(0);
}

/*
    This function loads the message from resourcebundle using the key passed
*/
String loadMessage(const char* key, const char* defMessage)
{
    MessageLoaderParms parms(key, defMessage);
    parms.msg_src_path = MSG_PATH;
    return MessageLoader::getMessage(parms);
}

/////////////////////////////////////////////////////////////////////
// The following section manages the input options.  It includes   //
// both the options processor (based on common/options             //
// and the options table.                                          //
/////////////////////////////////////////////////////////////////////

void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& testHome)
{
    static const char* outputFormats[] = { "xml", "mof", "txt", "table"};
    static const Uint32 NUM_OUTPUTFORMATS = sizeof(outputFormats) /
                                            sizeof(outputFormats[0]);
    static OptionRowWithMsg optionsTable[] =
        //optionname defaultvalue rqd  type domain domainsize clname msgkey
        // hlpmsg
    {
        {"count", "29346", false, Option::WHOLE_NUMBER, 0, 0, "count",
        "Clients.cimcli.CIMCLIClient.COUNT_OPTION_HELP",
        "Expected count of objects returned if summary set.\n"
            "    Test count and display difference.\n"
            "    Return nonzero if test fails"},

        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
        "Clients.cimcli.CIMCLIClient.DEBUG_OPTION_HELP",
        "More detailed debug messages"},

        {"delay", "0", false, Option::WHOLE_NUMBER, 0, 0, "delay",
        "Clients.cimcli.CIMCLIClient.DELAY_OPTION_HELP",
        "Delay between connection and request"},

        {"Password", "", false, Option::STRING, 0, 0, "p",
        "Clients.cimcli.CIMCLIClient.PASSWORD_OPTION_HELP",
        "Defines password for authentication" },

        {"location", "", false, Option::STRING, 0, 0, "l",
        "Clients.cimcli.CIMCLIClient.LOCATION_OPTION_HELP",
        "Specifies system and port (HostName:port).\n"
            "    Port is optional" },

#ifdef PEGASUS_HAS_SSL
        {"ssl", "false", false, Option::BOOLEAN, 0, 0, "s",
        "Clients.cimcli.CIMCLIClient.SSL_OPTION_HELP",
        "Specifies to connect over HTTPS" },

        {"clientCert", "", false, Option::STRING, 0, 0, "-cert",
        "Clients.cimcli.CIMCLIClient.CLIENTCERT_OPTION_HELP",
        "Specifies a client certificate to present to the server.\n"
            "    This is optional and only has an effect on connections\n"
            "    made over HTTPS using -s" },

        {"clientKey", "", false, Option::STRING, 0, 0, "-key",
        "Clients.cimcli.CIMCLIClient.CLIENTKEY_OPTION_HELP",
        "Specifies a client private key.\n"
            "    This is optional and only has an effect on connections\n"
            "    made over HTTPS using -s" },
#endif
        {"User", "", false, Option::STRING, 0, 0, "u",
        "Clients.cimcli.CIMCLIClient.USER_OPTION_HELP",
        "Defines User Name for authentication" },

        {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
        "Clients.cimcli.CIMCLIClient.NAMESPACE_OPTION_HELP",
        "Specifies namespace to use for operation" },

        {"deepInheritance", "false", false, Option::BOOLEAN, 0, 0, "di",
        "Clients.cimcli.CIMCLIClient.DEEPINHERITANCE_OPTION_HELP",
        "If set deepInheritance parameter\n"
            "    set true"},

        {"localOnly", "true", false, Option::BOOLEAN, 0, 0, "lo",
        "Clients.cimcli.CIMCLIClient.LOCALONLY_OPTION_HELP",
        "DEPRECATED. This was used to set LocalOnly.\n"
            "    However, default should be true and we cannot use True\n"
            "    as default. See -nlo"},

        {"notLocalOnly", "false", false, Option::BOOLEAN, 0, 0, "nlo",
        "Clients.cimcli.CIMCLIClient.NOTLOCALONLY_OPTION_HELP",
        "When set, sets LocalOnly = false on\n"
            "    operations"},

        {"includeQualifiers", "true", false, Option::BOOLEAN, 0, 0, "iq",
        "Clients.cimcli.CIMCLIClient.INCLUDEQUALIFIERS_OPTION_HELP",
        "Deprecated. Sets includeQualifiers = True.\n"
            "    However, default=true"},

        {"notIncludeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "niq",
        "Clients.cimcli.CIMCLIClient.NOTINCLUDEQUALIFIERS_OPTION_HELP",
        "Sets includeQualifiers = false\n"
            "    on operations"},

        // Uses a magic string as shown below to indicate never used.
        {"propertyList", "###!###", false, Option::STRING, 0, 0, "pl",
        "Clients.cimcli.CIMCLIClient.PROPERTYLIST_OPTION_HELP",
        "Defines a propertyNameList. Format is p1,p2,p3\n"
            "    (without spaces). Use \"\" for empty"},

        {"assocClass", "", false, Option::STRING, 0, 0, "ac",
        "Clients.cimcli.CIMCLIClient.ASSOCCLASS_OPTION_HELP",
        "Defines a assocClass string for Associator calls"},

        {"assocRole", "", false, Option::STRING, 0, 0, "ar",
        "Clients.cimcli.CIMCLIClient.ASSOCROLE_OPTION_HELP",
        "Defines a role string for Associatiors AssocRole\n"
            "    parameter"},

        {"role", "", false, Option::STRING, 0, 0, "r",
        "Clients.cimcli.CIMCLIClient.ROLE_OPTION_HELP",
        "Defines a role string for reference role parameter"},

        {"resultClass", "", false, Option::STRING, 0, 0, "rc",
        "Clients.cimcli.CIMCLIClient.RESULTCLASS_OPTION_HELP",
        "Defines a resultClass string for References and\n"
            "    Associatiors"},

        {"resultRole", "", false, Option::STRING, 0, 0, "rr",
        "Clients.cimcli.CIMCLIClient.RESULTROLE_OPTION_HELP",
        "Defines a role string for associators operation resultRole\n"
            "    parameter"},

        {"inputParameters", "", false, Option::STRING, 0, 0, "ip",
        "Clients.cimcli.CIMCLIClient.INPUTPARAMETERS_OPTION_HELP",
        "Defines an invokeMethod input parameter list.\n"
            "    Format is p1=v1 p2=v2 .. pn=vn\n"
            "    (parameters are seperated by spaces)"},

        {"filter", "", false, Option::STRING, 0, 0, "f",
        "Clients.cimcli.CIMCLIClient.FILTER_OPTION_HELP",
        "Defines a filter to use for query. Single String input"},

        // This was never used.  Delete. KS
        //{"substitute", "", false, Option::STRING, 0, 0, "-s",
        //  "Defines a conditional substition of input parameters. ) "},

        // KS change the output formats to use the enum options function
        // Deprecate this function.
        {"outputformats", "mof", false, Option::STRING, 0,NUM_OUTPUTFORMATS,
        "o",
        "Clients.cimcli.CIMCLIClient.OUTPUTFORMATS_OPTION_HELP",
        "Output in xml, mof, txt, table"},

        {"xmlOutput", "false", false, Option::BOOLEAN, 0,0, "x",
        "Clients.cimcli.CIMCLIClient.XMLOUTPUT_OPTION_HELP",
        "Output objects in xml instead of mof format"},

        {"version", "false", false, Option::BOOLEAN, 0, 0, "-version",
        "Clients.cimcli.CIMCLIClient.VERSION_OPTION_HELP",
        "Displays software Version"},

        {"verbose", "false", false, Option::BOOLEAN, 0, 0, "v",
        "Clients.cimcli.CIMCLIClient.VERBOSE_OPTION_HELP",
        "Verbose Display. Includes Detailed Param Input\n"
            "    display"},

        {"summary", "false", false, Option::BOOLEAN, 0, 0, "-sum",
        "Clients.cimcli.CIMCLIClient.SUMMARY_OPTION_HELP",
        "Displays only summary count for enumerations,\n"
            "    associators, etc."},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
        "Clients.cimcli.CIMCLIClient.HELP_OPTION_HELP",
        "Prints help usage message"},

        {"full help", "false", false, Option::BOOLEAN, 0, 0, "-help",
        "Clients.cimcli.CIMCLIClient.FULLHELP_OPTION_HELP",
        "Prints full help message with commands, options,\n"
            "    examples"},

        {"help options", "false", false, Option::BOOLEAN, 0, 0, "ho",
        "Clients.cimcli.CIMCLIClient.HELPOPTIONS_OPTION_HELP",
        "Prints list of options"},

        {"help commands", "false", false, Option::BOOLEAN, 0, 0, "hc",
        "Clients.cimcli.CIMCLIClient.HELPCOMMANDS_OPTION_HELP",
        "Prints CIM Operation command list"},

        {"connecttimeout", "0", false, Option::WHOLE_NUMBER, 0, 0, "-timeout",
        "Clients.cimcli.CIMCLIClient.CONNECTIONTIMEOUT_OPTION_HELP",
        "Set the connection timeout in seconds."},

        {"interactive", "false", false, Option::BOOLEAN, 0, 0, "i",
        "Clients.cimcli.CIMCLIClient.INTERACTIVE_OPTION_HELP",
        "Interactively ask user to select instances.\n"
            "    Used with associator and reference operations"},

        {"trace", "0", false, Option::WHOLE_NUMBER, 0, 0, "trace",
        "Clients.cimcli.CIMCLIClient.TRACE_OPTION_HELP",
        "Set Pegasus Common Components Trace. Sets the Trace level.\n"
            "    0 is off"},

        {"repeat", "0", false, Option::WHOLE_NUMBER, 0, 0, "-r",
        "Clients.cimcli.CIMCLIClient.REPEAT_OPTION_HELP",
        "Number of times to repeat the function.\n"
            "    Zero means one time"},

        {"time", "false", false, Option::BOOLEAN, 0, 0, "-t",
        "Clients.cimcli.CIMCLIClient.TIME_OPTION_HELP",
        "Measure time for the operation and present results"}

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = "cimcli.conf";

    //cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
    {
        om.mergeFile(configFile);
    }
    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}

/* Remap a long string into a string that can be positioned on a line
   starting at pos and with length but broken into multiple lines.
   The input string is recreated and filled from the left so that the
   returned string can be output as a multiline string starting at pos.
*/
String formatLongString (const char * input, Uint32 pos, Uint32 length)
{
    String output;
    String work = input;
    Array<String> list;

    // create the fill string starting with the newline character
    String fill;
    fill.append("\n");
    for (Uint32 i = 0; i < pos; i++)
        fill.append (" ");

    list = _tokenize(work, ' ');

    for (Uint32 i = 0 ; i < list.size() ; i++)
    {
        // move a single word and either a space or create new line
        if (((output.size() % length) + list[i].size()) >= (length))
            output.append(fill);
        else
            output.append(" ");

        output.append(list[i]);
    }
    return(output);
}

void showUsage()
{
    String usage;
    usage = 
        "Usage: cimcli <command> <CIMObject> <Options> *<extra parameters>\n"
        "    -hc    for <command> set and <CimObject> for each command\n"
        "    -ho    for <Options> set\n"
        "    -h xx  for <command> and <Example> for <xx> operation \n"
        "    -h     for this summary\n"
        "    --help for full help\n";
    CString str = usage.getCString();
    cout << loadMessage("Clients.cimcli.CIMCLIClient.MENU.STANDARD",
        (const char*)str);
}
/* showCommands - Display the list of operation commands.
*/
const char * helpSummary = 
    " -h for all help, -hc for commands, -ho for options";
void showCommands(const char* pgmName)
{
    for( Uint32 i = 0; i < NUM_COMMANDS; i++ )
    {
        char * cmdStr= new char[500];
        String txtFormat = formatLongString(
            CommandTable[i].UsageText,
            28,
            75 - 28 );
        CString ctxtFormat=txtFormat.getCString();
        sprintf(
            cmdStr,
            "\n%-5s %-21s",
            CommandTable[i].ShortCut,
            CommandTable[i].CommandName);
        cmdStr = strcat(cmdStr, (const char*)ctxtFormat);
        cout << loadMessage(
            CommandTable[i].msgKey,
            const_cast<const char*>(cmdStr)) 
            << endl;

        delete[] cmdStr;
    }
    cout << loadMessage(
        "Clients.cimcli.CIMCLIClient.HELP_SUMMARY",
        helpSummary) 
        << endl;

}

void showVersion(const char* pgmName, OptionManager& om)
{
    String str = "";
    str.append("Version ");
    str.append(PEGASUS_PRODUCT_VERSION);
   
    CString cstr = str.getCString();
    MessageLoaderParms parms(
        "Clients.cimcli.CIMCLIClient.VERSION", 
        (const char*)cstr,
        PEGASUS_PRODUCT_VERSION);
    parms.msg_src_path = MSG_PATH;
    cout << MessageLoader::getMessage(parms) << endl;
}

void showOptions(const char* pgmName, OptionManager& om)
{

    String optionsTrailer = loadMessage(
        "Clients.cimcli.CIMCLIClient.OPTIONS_TRAILER",
        "Options vary by command consistent with CIM Operations");
    cout << loadMessage(
        "Clients.cimcli.CIMCLIClient.OPTIONS_HEADER",
        "The options for this command are:\n");
    String usageStr;
    usageStr = loadMessage(
        "Clients.cimcli.CIMCLIClient.OPTIONS_USAGE",
        usage);
    om.printOptionsHelpTxt(usageStr, optionsTrailer);
}

/* PrintHelpMsg - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void printHelpMsg(
    const char* pgmName,
    const char* usage_,
    OptionManager& om)
{
    showUsage();

    showVersion(pgmName, om);

    showOptions(pgmName, om);

    showCommands(pgmName);

    cout << endl;

    showExamples();
}

void printUsageMsg(const char* pgmName,OptionManager& om)
{
    printHelpMsg(pgmName, usage, om);
}

int CheckCommonOptionValues(OptionManager& om, char** argv, Options& opts)
{
    // Catch the verbose and debug options first so they can control other
    // processing
    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;
    Boolean debug = (om.valueEquals("debug", "true")) ? true :false;

    if (verboseTest)
        opts.verboseTest = verboseTest;

    if (debug)
    {
        opts.debug= debug;
    }

    // Base code for parameter substition.  Dispabled until complete
    /*{
        String target;
        if (om.lookupValue("substitution", target))
        {
            // Get directory for file
            Array<String> fileList;
            String fileName = "/conditions.txt";
            String temp = ".";
            temp.append(fileName);
            fileList.append(temp);
            const char* tmpDir = getenv ("PEGASUS_HOME");
            if (tmpDir != NULL)
            {
                temp = tmpDir;
                temp.append(fileName);
                fileList.append(temp);
            }
    }*/


    if (om.isTrue("full help"))
    {
        printHelpMsg(argv[0], usage, om);
        exit(0);
    }

    if (om.isTrue("help"))
    {
        if (argv[1])
        {
            for (Uint32 i=0; i<NUM_COMMANDS;i++)
            {
                if (strcmp(argv[1], CommandTable[i].ShortCut) == 0)
                {
                    char * cmdStr= new char[500];
                    String txtFormat = formatLongString(
                        CommandTable[i].UsageText,28 ,75 - 28 );
                    CString ctxtFormat=txtFormat.getCString();
                    sprintf(
                        cmdStr, 
                        "\n%-5s %-21s",
                        CommandTable[i].ShortCut,
                        CommandTable[i].CommandName);
                    cmdStr = strcat(cmdStr, (const char*)ctxtFormat);
                    cout << loadMessage(
                        CommandTable[i].msgKey, 
                        const_cast<const char*>(cmdStr)) 
                        << endl;

                    delete[] cmdStr;

                    cout << loadMessage(
                        "Clients.cimcli.CIMCLIClient.EXAMPLE_STRING",
                        "Example : ") 
                        << endl;
                    cout << loadMessage(
                        examples[i].msgKey, 
                        examples[i].Example) 
                        << endl;
                    cout << loadMessage(
                        "Clients.cimcli.CIMCLIClient.OPTIONS_STRING",
                        optionText) 
                        << endl;
                    cout << loadMessage(
                        examples[i].optionsKey,
                        examples[i].Options) 
                        << endl;

                    char * commonOptStr = new char[100];
                    sprintf(commonOptStr, "%s", "Common Options are : \n");
                    commonOptStr = strcat(commonOptStr, commonOptions);
                    cout << loadMessage(
                        "Clients.cimcli.CIMCLIClient."
                            "COMMON_OPTIONS_STRING",
                        commonOptStr) 
                        << endl;
                    delete[] commonOptStr;
                    exit(0);
                }
            }
            cout << "Command not found. Type cimcli -hc to list valid commands."
                 << endl;
            exit(1);
        }

        showUsage();
        exit(0);
    }

    if (om.isTrue("version"))
    {
        showVersion(argv[0], om);
        exit(0);
    }

    if (om.isTrue("help options"))
    {
        showOptions(argv[0], om);
        exit(0);
    }

    if (om.isTrue("help commands"))
    {
        showCommands(argv[0]);
        exit(0);
    }

    // Establish the namespace from the input parameters
    //String nameSpace;
    if(om.lookupValue("namespace", opts.nameSpace))
    {
        if (verboseTest && debug)
            cout << "Namespace = " << opts.nameSpace << endl;
    }

    String temprole;
    if(om.lookupValue("role", temprole))
    {
        // we need to deliver String::EMPTY when no param.
        if (temprole != "")
            opts.role = temprole;
        if (verboseTest && debug && temprole != "")
           cout << "role = " << opts.role << endl;
    }

    String tempResultRole;
    if(om.lookupValue("resultRole", tempResultRole))
    {
        // we need to deliver String::EMPTY when no param.
        if (tempResultRole != "")
            opts.resultRole = tempResultRole;
        if (verboseTest && debug && tempResultRole != "")
           cout << "resultRole= " << opts.resultRole << endl;
    }

    // Get value for location, i.e. host, etc.
    om.lookupValue("location", opts.location);

#ifdef PEGASUS_HAS_SSL
    // Determine whether to connect over HTTPS
    opts.ssl = om.isTrue("ssl");

    // Get value for client certificate
    om.lookupValue("clientCert", opts.clientCert);

    // Get value for client key
    om.lookupValue("clientKey", opts.clientKey);

    if (verboseTest && debug && opts.ssl)
    {
        cout << "ssl = true" << endl;
        if (opts.clientCert != "" && opts.clientKey != "")
        {
            cout << "clientCert = " << opts.clientCert << endl;
            cout << "clientKey = " << opts.clientKey << endl;
        }
    }
#endif

    // Assign the result class
    if(om.lookupValue("resultClass", opts.resultClassName))
    {

       if (opts.resultClassName != "")
       {
           //Covers fact that assigning to CIMName can cause exception.
           try
           {
               opts.resultClass = opts.resultClassName;
           }
           catch(Exception& e)
           {
               cout << "Error in Result Class. Exception " << e.getMessage() 
                   << endl;
               exit(1);
           }
       }
       if (verboseTest && debug && opts.resultClassName != "")
           cout << "resultClassName = " << opts.resultClassName << endl;
    }

    if(om.lookupValue("assocClass", opts.assocClassName))
    {
       if (verboseTest && debug && opts.assocClassName != "")
           cout << "assocClassName = " << opts.assocClassName << endl;
       if (opts.assocClassName != "")
       {
           try
           {
               opts.assocClass = opts.assocClassName;
           }
           catch(Exception& e)
           {
               cout << "Error in assoc Class. Exception " << e.getMessage() 
                   << endl;
               exit(1);
           }
       }
    }

    // Evaluate connectiontimeout option.
    opts.connectionTimeout = 0;
    if (!om.lookupIntegerValue("connecttimeout", opts.connectionTimeout))
    {
        opts.connectionTimeout = 0;
    }

    if (verboseTest && debug && opts.connectionTimeout != 0)
    {
        cout << "Connection Timeout= " << opts.connectionTimeout << " Seconds" 
            << endl;
    }

    if (!om.lookupIntegerValue("delay", opts.delay))
    {
        opts.delay = 0;
    }

    if (verboseTest && debug && opts.delay != 0)
    {
        cout << "delay= " << opts.delay << " Seconds" << endl;
    }

    // Set the interactive request flag based on input
    opts.interactive = om.isTrue("interactive");
    if (om.isTrue("interactive")  && verboseTest && debug)
        cout << "interactive request" << endl;

    // set the deepInheritance flag based on input
    opts.deepInheritance = om.isTrue("deepInheritance");
    if (om.isTrue("deepInheritance")  && verboseTest && debug)
        cout << "deepInteritance set" << endl;

    // process localOnly and notlocalOnly parameters
    opts.localOnly = om.isTrue("localOnly");
    if (om.isTrue("notLocalOnly"))
    {
        opts.localOnly = false;
    }

    // Used the not version because the DMTF and pegasus default is true
    if (verboseTest && debug && om.isTrue("notLocalOnly"))
    {
        cout << "localOnly= " << _toString(opts.localOnly) << endl;;
    }

    // Process includeQualifiers and notincludeQualifiers
    opts.includeQualifiers = om.isTrue("includeQualifiers");

    if (om.isTrue("notIncludeQualifiers"))
    {
        opts.includeQualifiers = false;
    }

    if (verboseTest && debug && om.isTrue("notIncludeQualifiers"))
    {
        cout << "includeQualifiers = " << _toString(opts.includeQualifiers) 
            << endl;
    }

    opts.includeClassOrigin = om.isTrue("includeClassOrigin");
    if (om.isTrue("includeClassOrigin")  && verboseTest && debug)
        cout << "includeClassOrigin set" << endl;

    opts.time = om.isTrue("time");
    if (om.isTrue("time")  && verboseTest && debug)
        cout << "time set" << endl;

    if(!om.lookupIntegerValue("trace", opts.trace))
    {
            opts.trace = 0;
    }
    else
    {
        Uint32 traceLevel = 0;
        switch (opts.trace)
        {
            case 0:             // This covers the default.
                break;
            case 1 :
                traceLevel = Tracer::LEVEL1;
                break;
            case 2 :
                traceLevel = Tracer::LEVEL2;
                break;
            case 3 :
                traceLevel = Tracer::LEVEL3;
                break;
            case 4 :
                traceLevel = Tracer::LEVEL4;
                break;
            default:
                cout << "Illegal value for Trace. Max = 4" << endl;
        }
        opts.trace = traceLevel;
    }
    if (verboseTest && debug && opts.trace != 0)
    {
        cout << "Pegasus Trace set to  Level  " << opts.trace << endl;
    }

    opts.summary = om.isTrue("summary");

    // get User name and password if set.
     if(om.lookupValue("User", opts.user))
     {
         if (opts.user.size() == 0)
             opts.user = String::EMPTY;
         if (debug && verboseTest)
             cout << "User= " << opts.user << endl;
     }

     if(om.lookupValue("Password", opts.password))
     {
         if (opts.password.size() == 0)
             opts.password = String::EMPTY;
         if (debug && verboseTest)
             cout << "Password= " << opts.password << endl;
     }

    // Create a variable with the format output and a correponding type.
    // Suggest we might change this whole thing to the option type that
    // mike used in the example of colors so that  you could do -red -blue
    // or in our case -mof -xml, etc.

     opts.isXmlOutput = om.isTrue("xmlOutput");
     if (opts.isXmlOutput  && debug && verboseTest)
         cout << "xmlOutput set" << endl;

    if(om.lookupValue("outputformats", opts.outputFormat))
     {
        if (debug && verboseTest)
            cout << "Output Format = " << opts.outputFormat << endl;
     }

    // Get the output format parameter and save it
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    for( ; cnt < NUM_OUTPUTS; cnt++ )
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    // Note that this makes no notice of a not found
    if (cnt != NUM_OUTPUTS)
    {
        opts.outputFormatType = cnt;
        opts.outputType = OutputTable[cnt].OutputType;
    }

    if(!om.lookupIntegerValue("repeat", opts.repeat))
    {
            opts.repeat = 0;

    }
    if (debug && verboseTest)
    {
        cout << "Repeat Count= " << opts.repeat << endl;
    }

    if(!om.lookupIntegerValue("count", opts.count))
    {
            opts.count = 29346;
    }
    if (debug && verboseTest)
    {
        if (opts.count != 29346)
        {
        cout << "Comparison Count= " << opts.count << endl;
        }
    }

    /*  Property List parameter.
        Separate an input stream into an array of Strings
        Two special situations, empty list and NULL list
        Use NULL when there is no list.
        Use empty if
    */
    {
        String properties;
        if(om.lookupValue("propertyList", properties))
        {
            if (properties == "###!###")
            {
                opts.propertyList.clear();
            }
            else
            {
                Array<CIMName> pList;
                Array<String> pListString =  _tokenize(properties, ',');

                for (Uint32 i = 0 ; i < pListString.size(); i++)
                {
                    pList.append(CIMName(pListString[i]));
                }
                opts.propertyList.set(pList);
            }
            if (debug && verboseTest && properties != "###!###")
            {
                cout << "PropertyList= ";
                printPropertyList(opts.propertyList);
                cout << endl;
            }
        }
    }

    /* Method input parameters processing.  Process as one
       string containing multiple parameters in the form
       name=value
       ATTN: KS 20030426 Note that we have not accounted for
       the typing on parameters.
    */
    String inputParameters;
    if(om.lookupValue("inputParameters", inputParameters))
    {
        // first tokenization is the ' '
        Array<String> pList =  _tokenize(inputParameters, ' ');
        for (Uint32 i = 0 ; i< pList.size() ; i++)
        {
            CIMParamValue pv;
            pv = _createMethodParamValue(pList[i], opts);
            opts.inParams.append(pv);
        }
    }
    return 0;
}


//------------------------------------------------------------------------------
//
// mofFormat
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentSize)
{
    Uint32 n = level * indentSize;
    if (n > 50)
    {
    cout << "Jumped Ship " << level << " size " << indentSize << endl;
    exit(1);
    }

    for (Uint32 i = 0; i < n; i++)
        os << ' ';
}

/* Format the output stream for indented MOF format
*/
void mofFormat(
    PEGASUS_STD(ostream)& os,
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

void _printTables(
    const Array<Uint32>& maxColumnWidth,
    const Array<ColumnEntry>& outputTable,
    PEGASUS_STD(ostream)& outPrintWriter)
{
    for (Uint32 i = 0; i < outputTable[0].size(); i++)
    {
        for (Uint32 column = 0; column < maxColumnWidth.size() - 1; column++)   
        {
            Uint32 fillerLen = maxColumnWidth[column] - 
                outputTable[column][i].size();

            outPrintWriter << outputTable[column][i];

            for (Uint32 j = 0; j < fillerLen + 2; j++)
            {
                 outPrintWriter << ' ';
            }
        }
        outPrintWriter << outputTable[maxColumnWidth.size() - 1][i] << endl;
    }    
}

/* Format the output stream to be table format
*/
void tableFormat(
    PEGASUS_STD(ostream)& outPrintWriter,
    const Array<CIMInstance>& instances)
{
    Array<ColumnEntry> outputTable;
    Array<Uint32> maxColumnWidth;    

    for (Uint32 i = 0; i < instances[0].getPropertyCount(); i++)
    {
        Array<String> property;

        String propertyNameStr = 
            instances[0].getProperty(i).getName().getString();
        property.append(propertyNameStr);

        maxColumnWidth.append(propertyNameStr.size());

        for (Uint32 j = 0; j < instances.size(); j++)
        {
            String propertyValueStr = 
                instances[j].getProperty(i).getValue().toString();
            property.append(propertyValueStr);

            if (propertyValueStr.size() > maxColumnWidth[i])
            {
                maxColumnWidth[i] = propertyValueStr.size();
            }
        }

        outputTable.append(property);
    }

    _printTables(maxColumnWidth, outputTable, outPrintWriter);
}

PEGASUS_NAMESPACE_END
// END_OF_FILE


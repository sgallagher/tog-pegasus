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

#include <cstdlib>
#include <cctype>
#include <fstream>
#include <cstdio>
#include "OptionManager.h"
#include "Destroyer.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T OptionPtr
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// TODO: add descriptions for each otpion so that they may be printed out
// to the command line user to tell him which command line options are legal.
//
// TODO: expand varaibles in the configuration file. For example:
//
//     provider_dir = "${home}/providers"
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Option
//
////////////////////////////////////////////////////////////////////////////////

const Array<String>& Option::getDomain() const
{
    return _domain;
}

void Option::setDomain(const Array<String>& domain)
{
    _domain = domain;
}

////////////////////////////////////////////////////////////////////////////////
//
// OptionManager
//
////////////////////////////////////////////////////////////////////////////////

OptionManager::OptionManager()
{

}

OptionManager::~OptionManager()
{
    // Delete all options in the list:

    for (Uint32 i = 0; i < _options.size(); i++)
	delete _options[i];
}

void OptionManager::registerOption(Option* option)
{
    if (!option)
	throw NullPointer();

    if (lookupOption(option->getOptionName()))
	throw DuplicateOption(option->getOptionName());

    _options.append(option);
}

void OptionManager::registerOptions(OptionRow* optionRow, Uint32 numOptions)
{
    for (Uint32 i = 0; i < numOptions; i++)
    {
	// Get option name:

	if (!optionRow[i].optionName)
	    throw NullPointer();

	String optionName = optionRow[i].optionName;

	// Get default value:

	String defaultValue;

	if (optionRow[i].defaultValue)
	    defaultValue = optionRow[i].defaultValue;

	// Get the required flag:

	Boolean required = optionRow[i].required != 0;

	// Get the type:

	Option::Type type = optionRow[i].type;

	// Get the domain:

	Array<String> domain;

	if (optionRow[i].domain)
	{
	    Uint32 domainSize = optionRow[i].domainSize;

	    for (Uint32 j = 0; j < domainSize; j++)
		domain.append(optionRow[i].domain[j]);
	}

	// Get commandLineOptionName:

	String commandLineOptionName;

	if (optionRow[i].commandLineOptionName)
	    commandLineOptionName = optionRow[i].commandLineOptionName;

	// get optionHelp Message String

	String optionHelpMessage;

	if (optionRow[i].optionHelpMessage)
	    optionHelpMessage = optionRow[i].optionHelpMessage;

	// Add the option:

	Option* option = new Option(
	    optionName,
	    defaultValue,
	    required,
	    type,
	    domain,
	    commandLineOptionName,
	    optionHelpMessage);

	registerOption(option);
    }
}

void OptionManager::mergeCommandLine(int& argc, char**& argv)
{
    for (int i = 0; i < argc; )
    {
	// Check for -option:

	const char* arg = argv[i];

	if (*arg == '-')
	{
	    // Look for the option:

	    Option* option = _lookupOptionByCommandLineOptionName(arg + 1);

	    if (!option)
	    {
		i++;
		continue;
	    }

	    // Get the option argument if any:

	    const char* optionArgument = "true";

	    if (option->getType() != Option::BOOLEAN)
	    {
		if (i + 1 == argc)
		    throw MissingCommandLineOptionArgument(arg);

		optionArgument = argv[i + 1];
	    }

	    // Validate the value:

	    if (!option->isValid(optionArgument))
		throw InvalidOptionValue(arg, optionArgument);

	    // Set the value:

	    option->setValue(optionArgument);

	    // Remove the option and its argument from the command line:

	    if (option->getType() == Option::BOOLEAN)
	    {
		memmove(&argv[i], &argv[i + 1], (argc-i) * sizeof(char*));
		argc--;
	    }
	    else
	    {
		memmove(&argv[i], &argv[i + 2], (argc-i-1) * sizeof(char*));
		argc -= 2;
	    }
	}
	else
	    i++;
    }
}

void OptionManager::mergeFile(const String& fileName)
{
    // Open the input file:

    ArrayDestroyer<char> p(fileName.allocateCString());
    ifstream is(p.getPointer());

    if (!is)
	throw NoSuchFile(fileName);

    // For each line of the file:

    String line;

    for (Uint32 lineNumber = 1; GetLine(is, line); lineNumber++)
    {
	// -- Get the identifier and value:

	if (line[0] == '#')
	    continue;

	// Skip leading whitespace:

	const Char16* p = line.getData();

	while (*p && isspace(*p))
	    p++;

	if (!*p)
	    continue;

	if (*p == '#')
	    continue;

	// Get the identifier:

	String ident;

	if (!(isalpha(*p) || *p == '_'))
	    throw ConfigFileSyntaxError(fileName, lineNumber);

	ident += *p++;

	while (isalnum(*p) || *p == '_')
	    ident += *p++;

	// Skip whitespace after identifier:

	while (*p && isspace(*p))
	    p++;

	// Expect an equal sign:

	if (*p != '=')
	    throw ConfigFileSyntaxError(fileName, lineNumber);
	p++;

	// Skip whitespace after equal sign:

	while (*p && isspace(*p))
	    p++;

	// Expect open quote:

	if (*p != '"')
	    throw ConfigFileSyntaxError(fileName, lineNumber);
	p++;

	// Get the value:

	String value;

	while (*p && *p != '"')
	{
	    if (*p == '\\')
	    {
		p++;

		switch (*p)
		{
		    case 'n': 
			value += '\n'; 
			break;
		    
		    case 'r':
			value += '\r';
			break;

		    case 't':
			value += '\t';
			break;

		    case 'f':
			value += '\f';
			break;

		    case '"':
			value += '"';
			break;

		    case '\0':
			throw ConfigFileSyntaxError(fileName, lineNumber);
			break;

		    default:
			value += *p;
		}
		p++;
	    }
	    else
		value += *p++;
	}


	// Expect close quote:

	if (*p != '"')
	    throw ConfigFileSyntaxError(fileName, lineNumber);
	p++;

	// Skip whitespace through end of line:

	while (*p && isspace(*p))
	    p++;

	if (*p)
	    throw ConfigFileSyntaxError(fileName, lineNumber);

	// Now that we have the identifier and value, merge it:

	Option* option = (Option*)lookupOption(ident);

	if (!option)
	    throw UnrecognizedConfigFileOption(ident);

	if (!option->isValid(value))
	    throw InvalidOptionValue(ident, value);

	option->setValue(value);
    }
}

void OptionManager::checkRequiredOptions() const
{
    for (Uint32 i = 0; i < _options.size(); i++)
    {
	const Option* option = _options[i];

	if (option->getRequired() && !option->isResolved())
	    throw MissingRequiredOptionValue(option->getOptionName());
    }
}

const Option* OptionManager::lookupOption(const String& name) const
{
    for (Uint32 i = 0; i < _options.size(); i++)
    {
	if (_options[i]->getOptionName() == name)
	    return _options[i];
    }

    return 0;
}

Boolean OptionManager::lookupValue(const String& name, String& value) const
{
    const Option* option = lookupOption(name);

    if (!option)
	return false;

    value = option->getValue();
    return true;
}

Boolean OptionManager::valueEquals(const String& name, const String& value) 
    const
{
    String optionString;

    if (lookupValue(name, optionString) && optionString == value)
	return true;
    else
	return false;

}
/*  Buried this one for the moment to think about it.
Uint32 OptionManager::isStringInOptionMask(const String& option, 
					   const String& entry) 
{
    String optionString;

    if (lookupValue(name, optionString) && optionString == value)
	if (optionString.find(entry)
	    return 1;
    else
	return PEG_NOT_FOUND;
}
*/

Option* OptionManager::_lookupOptionByCommandLineOptionName(const String& name)
{
    for (Uint32 i = 0; i < _options.size(); i++)
    {
	if (_options[i]->getCommandLineOptionName() == name)
	    return _options[i];
    }

    return 0;
}

void OptionManager::print() const
{
    for (Uint32 i = 0; i < _options.size(); i++)
    {
	Option* option = _options[i];
	cout << option->getOptionName() << "=\"";
	cout << option->getValue() << "\" ";
	cout << option->getOptionHelpMessage() << "\n";
    }
    cout << endl;
}

void OptionManager::printHelp() const
{
    for (Uint32 i = 0; i < _options.size(); i++)
    {
	Option* option = _options[i];
	cout << " -";
	cout << option->getCommandLineOptionName() << "  ";
	cout << option->getOptionName() << " ";
	cout << option->getOptionHelpMessage() << "\n";
    }
    cout << endl;

}

////////////////////////////////////////////////////////////////////////////////
//
// Option
//
////////////////////////////////////////////////////////////////////////////////

Option::Option(
    const String& optionName,
    const String& defaultValue,
    Boolean required,
    Type type,
    const Array<String>& domain,
    const String& commandLineOptionName,
    const String& optionHelpMessage)
    :
    _optionName(optionName),
    _defaultValue(defaultValue),
    _value(defaultValue),
    _required(required),
    _type(type),
    _domain(domain),
    _commandLineOptionName(commandLineOptionName),
    _optionHelpMessage(optionHelpMessage),
    _resolved(false)
{
    if (!isValid(_value))
	throw InvalidOptionValue(_optionName, _value);
}

Option::Option(const Option& x) 
    :
    _optionName(x._optionName),
    _defaultValue(x._defaultValue),
    _value(x._value),
    _required(x._required),
    _type(x._type),
    _domain(x._domain),
    _commandLineOptionName(x._commandLineOptionName),
    _optionHelpMessage(x._optionHelpMessage)

{
}

Option::~Option()
{

}

Option& Option::operator=(const Option& x)
{
    if (this != &x)
    {
	_optionName = x._optionName;
	_defaultValue = x._defaultValue;
	_value = x._value;
	_required = x._required;
	_type = x._type;
	_domain = x._domain;
	_commandLineOptionName = x._commandLineOptionName;
	_optionHelpMessage = x._optionHelpMessage;
    }
    return *this;
}

Boolean Option::isValid(const String& value) const
{
    // Check to see that the value is in the domain (if a domain was given)

    Uint32 domainSize = _domain.size();

    if (domainSize)
    {
	Boolean found = false;

	for (Uint32 i = 0; i < domainSize; i++)
	{
	    if (value == _domain[i])
		found = true;
	}

	if (!found)
	    return false;
    }

    // Check the type:

    switch (_type)
    {
	case BOOLEAN:
	{
	    if (value == "true" || value == "false")
		return true;
	}

	case STRING:
	    return true;

	case INTEGER:
	case NATURAL_NUMBER:
	case WHOLE_NUMBER:
	{
	    char* tmp = value.allocateCString();
	    char* end = 0;
	    long x = strtol(tmp, &end, 10);

	    if (!end || *end != '\0')
		return false;

	    delete [] tmp;

	    switch (_type)
	    {
		case INTEGER: 
		    return true;

		case NATURAL_NUMBER:
		    return x >= 1;

		case WHOLE_NUMBER:
		    return x >= 0;

		default:
		    break;
	    }
	}
    }

    // Unreachable!
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// ConfigFileSyntaxError
//
////////////////////////////////////////////////////////////////////////////////

String ConfigFileSyntaxError::_formatMessage(
    const String& file, Uint32 line)
{
    char buffer[32];
    sprintf(buffer, "%d", line);

    String result = "Syntax error in configuration file: ";
    result += file;
    result += "(";
    result += buffer;
    result += ")";
    return result;
}

PEGASUS_NAMESPACE_END

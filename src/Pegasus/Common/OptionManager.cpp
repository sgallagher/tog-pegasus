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
// Author: Michael E. Brasher
//
// $Log: OptionManager.cpp,v $
// Revision 1.6  2001/04/14 07:35:04  mike
// Added config file loading to OptionManager
//
// Revision 1.5  2001/04/14 06:41:17  mike
// New
//
// Revision 1.4  2001/04/14 03:37:16  mike
// Added new example to test option manager
//
// Revision 1.3  2001/04/14 02:26:42  mike
// More on option manager implementation
//
// Revision 1.2  2001/04/14 02:11:41  mike
// New option manager class.
//
// Revision 1.1  2001/04/14 01:52:45  mike
// New option management class.
//
//
//END_HISTORY

#include <cstdlib>
#include <fstream>
#include "OptionManager.h"
#include "Destroyer.h"

PEGASUS_NAMESPACE_BEGIN

using namespace std;

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

    for (Uint32 i = 0; i < _options.getSize(); i++)
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

void OptionManager::registerOptions(OptionRow* options, Uint32 numOptions)
{
    for (Uint32 i = 0; i < numOptions; i++)
    {
	// Get option name:

	if (!options[i].optionName)
	    throw NullPointer();

	String optionName = options[i].optionName;

	// Get default value:

	String defaultValue;

	if (options[i].defaultValue)
	    defaultValue = options[i].defaultValue;

	// Get the required flag:

	Boolean required = options[i].required;

	// Get the type:

	Option::Type type = options[i].type;

	// Get the domain:

	Array<String> domain;

	if (options[i].domain)
	{
	    Uint32 domainSize = options[i].domainSize;

	    for (Uint32 j = 0; j < domainSize; j++)
		domain.append(options[i].domain[j]);
	}

	// Get environmentVariableName:

	String environmentVariableName;

	if (options[i].environmentVariableName)
	    environmentVariableName = options[i].environmentVariableName;

	// Get configFileOptionName:

	String configFileOptionName;

	if (options[i].configFileOptionName)
	    configFileOptionName = options[i].configFileOptionName;

	// Get commandLineOptionName:

	String commandLineOptionName;

	if (options[i].commandLineOptionName)
	    commandLineOptionName = options[i].commandLineOptionName;

	// Add the option:

	Option* option = new Option(
	    optionName,
	    defaultValue,
	    required,
	    type,
	    domain,
	    environmentVariableName,
	    configFileOptionName,
	    commandLineOptionName);

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

void OptionManager::mergeEnvironment()
{
    // ATTN-A: Implement
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

	Option* option = _lookupOptionByConfigFileOptionName(ident);

	if (!option)
	    throw UnrecognizedConfigFileOption(ident);

	if (!option->isValid(value))
	    throw InvalidOptionValue(ident, value);

	option->setValue(value);
    }
}

void OptionManager::checkRequiredOptions() const
{
    // ATTN-A: Implement
}

const Option* OptionManager::lookupOption(const String& name) const
{
    for (Uint32 i = 0; i < _options.getSize(); i++)
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

Option* OptionManager::_lookupOptionByCommandLineOptionName(const String& name)
{
    for (Uint32 i = 0; i < _options.getSize(); i++)
    {
	if (_options[i]->getCommandLineOptionName() == name)
	    return _options[i];
    }

    return 0;
}

Option* OptionManager::_lookupOptionByConfigFileOptionName(const String& name)
{
    for (Uint32 i = 0; i < _options.getSize(); i++)
    {
	if (_options[i]->getConfigFileOptionName() == name)
	    return _options[i];
    }

    return 0;
}

void OptionManager::print() const
{
    for (Uint32 i = 0; i < _options.getSize(); i++)
    {
	Option* option = _options[i];
	cout << option->getOptionName() << "=\"";
	cout << option->getValue() << "\"\n";
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
    const StringArray& domain,
    const String& environmentVariableName,
    const String& configFileOptionName,
    const String& commandLineOptionName)
    :
    _optionName(optionName),
    _defaultValue(defaultValue),
    _value(defaultValue),
    _required(required),
    _type(type),
    _domain(domain),
    _environmentVariableName(environmentVariableName),
    _configFileOptionName(configFileOptionName),
    _commandLineOptionName(commandLineOptionName),
    _foundValue(false)
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
    _environmentVariableName(x._environmentVariableName),
    _configFileOptionName(x._configFileOptionName),
    _commandLineOptionName(x._commandLineOptionName)
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
	_environmentVariableName = x._environmentVariableName;
	_configFileOptionName = x._configFileOptionName;
	_commandLineOptionName = x._commandLineOptionName;
    }
    return *this;
}

Boolean Option::isValid(const String& value) const
{
    // Check to see that the value is in the domain (if a domain was given)

    Uint32 domainSize = _domain.getSize();

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
	    delete [] tmp;

	    if (!end || *end != '\0')
		return false;

	    switch (_type)
	    {
		case INTEGER: 
		    return true;

		case NATURAL_NUMBER:
		    return x >= 1;

		case WHOLE_NUMBER:
		    return x >= 0;
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

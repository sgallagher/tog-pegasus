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

#include "OptionManager.h"

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

Boolean OptionManager::registerOption(Option* option)
{
    if (!option)
	return false;

    if (lookupOption(option->getOptionName()))
	return false;

    _options.append(option);
    return true;
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
		throw BadCommandLineOption(arg);

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
    // ATTN-A: Implement
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

Option* OptionManager::_lookupOptionByCommandLineOptionName(const String& name)
{
    for (Uint32 i = 0; i < _options.getSize(); i++)
    {
	if (_options[i]->getCommandLineOptionName() == name)
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
    const String& configFileVariableName,
    const String& commandLineOptionName)
    :
    _optionName(optionName),
    _defaultValue(defaultValue),
    _value(defaultValue),
    _required(required),
    _type(type),
    _domain(domain),
    _environmentVariableName(environmentVariableName),
    _configFileVariableName(configFileVariableName),
    _commandLineOptionName(commandLineOptionName),
    _foundValue(false)
{

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
    _configFileVariableName(x._configFileVariableName),
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
	_configFileVariableName = x._configFileVariableName;
	_commandLineOptionName = x._commandLineOptionName;
    }
    return *this;
}

Boolean Option::isValid(const String& value) const
{
    // ATTN-A: Implement
    return true;
}

PEGASUS_NAMESPACE_END

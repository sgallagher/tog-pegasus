//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OptionManager_h
#define Pegasus_OptionManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class Option;
struct OptionRow;

/** The OptionManager class manages a collection of program options.

    <h4>Overview</h4>

    A program option may be specified in two ways:

    <ul>
	<li>In a configuration file</li>
	<li>On the command line</li>
    </ul>

    This class provides methods for merging options from both of these
    sources into a single collection. The following example shows how to
    merge options from a command line into the option manager.

    <pre>
	int main(int argc, char** argv)
	{
	    OptionManager om;

	    ...

	    // Merge options from the command line into the option manager's
	    // option list. Remove arguments from command line.

	    om.mergeCommandLine(argc, argv);

	    ...
	}
    </pre>

    Similarly, the OptionManager::mergeFile() method allows options to be
    merged from a file.

    Before options are merged into the option manager, information on each
    option must be registered with the option manager so that the following
    can be resolved:

    <ul>
	<li>The option's name</li>
	<li>The default value (to be used if not specified elsewhere)</li>
	<li>Whether the option is required</li>
	<li>The option's type (e.g., boolean, integer or string)</li>
	<li>The domain of the option if any (set of legal values)</li>
	<li>The name the option as it appears on the command line</li>
    </ul>

    Here is how to regsiter an option:

    <pre>
	OptionManager om;

	Option* option = new Option("port", "80", false,
	    Option::NATURAL_NUMBER, Array<String>(), "p");

	om.registerOption(option);
    </pre>

    The arguments of the Option constructor are the same (and in the same
    order) as the list just above. Notice the last argument is "p". This
    is the name of the option argument on the command line (it would appear
    as "-p" on the command line).

    Once options have been registered, the option values may be initialized
    using the merge methods described earlier. During merging, certain
    validation is done using the corresponding Option instance described above.

    Once options have been merged, they may obtained by calling the
    lookupOption() method like this:

    <pre>
	Option* option = om.lookupOption("port");
	String port = option->getValue();
    </pre>

    Or the lookupValue() convenience function may be used to lookup values:

    <pre>
	String value;
	om.lookupValue("port", value);
    </pre>

    <h4>Command Line Options</h4>

    mergeCommandLine() like this:

    <pre>
	om.mergeCommandLine(argc, argv);
    </pre>

    This method searches the command line for options that match the registered
    ones. It will extract those options from the command line (argc and argv
    will be modified accordingly).

    <h4>Configuration File Otpions</h4>

    Options from a configuration file may be merged by calling the mergeFile()
    method like this:

    <pre>
	om.mergeFile(fileName);
    </pre>

    This searches the file for options matching registered ones. Exceptions
    are thrown for any unrecognized option names that are encountered.

    <h4>Merge Validation</h4>

    During merging, the option manager validates the following (using the
    information optatined during option registration).

    <ul>
	<li>The type of the option - whether integer, positive integer,
	    or string or whatever.</li>
	<li>The domain of the option - whether the supplied option is a legal
	    value for that otpion</li>
	<li>User extended validation - whether the user overriden
	    Option::isValid() returns true when the value is passed to it</li>
    </ul>

    <h4>Typcial Usage</h4>

    The OptionManager is typically used in the following way. First, options
    are registered to establish the valid set of options. Next, values are
    merged from the various sources by calling the merge functions. Finally,
    checkRequiredOptions() is called to see if any required option values were
    not provided.
*/
class PEGASUS_COMMON_LINKAGE OptionManager
{
public:

    /** Constructor. */
    OptionManager();

    /** Destructor. Deletes all contained Options. */
    ~OptionManager();

    /** Registers an option. The OptionManager is responsible for disposing
	of the option; the caller must not delete this object.

	@param option option to be registerd.
	@exception NullPointer exception if option argument is null.
	@exception DuplicateOption if option already defined.
    */
    void registerOption(Option* option);

    /** Provides a simple way to register several options at once using
	a declartive style table. This may also be done programmitically
	by repeatedly calling registerOption above. See documentation for
	OptionRow for details on how to use them.
    */
    void registerOptions(OptionRow* options, Uint32 numOptions);

    /** Merge option values from the command line. Searches the command
	line for registered options whose names are given by the
	Option::getCommandLineOptionName() method. Validation is performed
	on each option value obtained by calling Option::isValid(). Valid
	option values are set by calling Option::setValue(). The argc and
	argv arguments are modified: the option and its argument are
	stripped from the command line. The option and its argument have the
	following form: -option-name argument. A space must be supplied
	between the two. Boolean option arguments are an exception. They
	must have the form -option. If they are present, then they are
	taken to be true.

	&param argc number of argument on the command line.
	&param argv list of command line arguments.
	&exception InvalidOptionValue if validation fails.
	&exception MissingCommandLineOptionArgument
    */
    void mergeCommandLine(int& argc, char**& argv);

    /** Merge option values from a file. Searches file for registered options
	whose names are given by the options which have been registered.
	Validation is performed on each option value by calling
	Option::isValid(). Valid option values are set by calling
	Option::setValue().

	&param fileName name of file to be merged.
	&exception NoSuchFile if file cannot be opened.
	&exception BadConfigFileOption
    */
    void mergeFile(const String& fileName);

    /** After merging, this method is called to check for required options
	that were not merged (specified).

	&exception MissingRequiredRequiredOption
    */
    void checkRequiredOptions() const;

    /** Lookup the option with the given name.
	@param Name provides the name of the option.
	@return 0 if no such option.
    */
    const Option* lookupOption(const String& name) const;

    /** Lookup value of an option.
	@param Name provides the name of the option (ex. "port")
	@param String parameter contains the String that contains the
	value for this parameter (in String format).
	@return Boolean return. True if the option found.
    */
    Boolean lookupValue(const String& name, String& value) const;

    /** Print all the options. */
    void print() const;

private:

    /** Lookup the option by its commandLineOptionName.
	@return 0 if no such option.
    */
    Option* _lookupOptionByCommandLineOptionName(const String& name);

    Array<Option*> _options;
};

/** The Option class is used to specify information about an Option.

    See the OptionManager class for more details.
*/
class PEGASUS_COMMON_LINKAGE Option
{
public:

    /** Valid value types. */
    enum Type
    {
	// (..., -3, -2, -1, 0, 1, 2, 3, ...)
	INTEGER,

	// (1, 2, 3, ...)
	NATURAL_NUMBER,

	// (0, 1, 2, 3, ...)
	WHOLE_NUMBER,

	// "true" or "false"
	BOOLEAN,

	// Anything
	STRING
    };

    /** Constructor.

	@param optionName the name of this option.

	@param defaultValue the default value of this option.

	@param required whether the value of this option is required.

	@param type type of the value. This is used to validate the value.

	@param domain list of legal value for this option. If this list
	    is empty, then no domain is enforced.

	@param commandLineOptionName name of the corresponding command line
	    option (which may be different from the option name).
    */
    Option(
        const String& optionName,
        const String& defaultValue,
        Boolean required,
        Type type,
        const Array<String>& domain = EmptyStringArray(),
        const String& commandLineOptionName = String());

    Option(const Option& x);

    virtual ~Option();

    Option& operator=(const Option& x);

    /** Accessor */
    const String& getOptionName() const
    {
        return _optionName;
    }

    /** Modifier */
    void setOptionName(const String& optionName)
    {
        _optionName = optionName;
    }

    /** Accessor */
    const String& getDefaultValue() const
    {
        return _defaultValue;
    }

    /** Modifier. */
    void setDefaultValue(const String& defaultValue)
    {
        _defaultValue = defaultValue;
    }

    /** Accessor
	@return - Returns string representation of value
    */
    const String& getValue() const
    {
        return _value;
    }

    /** Modifier */
    void setValue(const String& value)
    {
        _value = value;
	_resolved = true;
    }

    /** Accessor */
    Boolean getRequired() const
    {
        return _required;
    }

    /** Modifier */
    void setRequired(Boolean required)
    {
        _required = required;
    }

    /** Accessor */
    Type getType() const
    {
        return _type;
    }

    /** Modifier */
    void setType(Type type)
    {
        _type = type;
    }

    /** Accessor */
    const Array<String>& getDomain() const;

    /** Modifier */
    void setDomain(const Array<String>& domain);

    /** Accessor */
    const String& getCommandLineOptionName() const
    {
        return _commandLineOptionName;
    }

    /** Modifier */
    void setCommandLineOptionName(const String& commandLineOptionName)
    {
        _commandLineOptionName = commandLineOptionName;
    }

    /** Accesor. Returns true if an option value was ever obtained for
	this option.
    */
    Boolean isResolved() const
    {
	return _resolved;
    }

    /** Checks to see if the given value is valid or not. This method may be
	overriden by derived classes to do special purpose validation of the
	value. This implementation just checks the domain and type.
    */
    virtual Boolean isValid(const String& value) const;

private:
    String _optionName;
    String _defaultValue;
    String _value;
    Boolean _required;
    Type _type;
    Array<String> _domain;
    String _commandLineOptionName;
    Boolean _resolved;
};

/** The OptionRow provides a declarative way of defining Option objects.
    For the declarative programming enthusiast, we provide this structure.
    It provides a declarative way of defining options for the OptionManager
    class. Some developers prefer this since it makes all the options visible
    in a kind of table like structure. Here is an example of how it can be
    used to define a port number and hostname options. We also show how to
    register one of these option lists with an OptionManager.

    <pre>
	static OptionRow options[] =
	{
	    { "port", "80", false, Option::NATURAL_NUMBER },
	    { "hostname", "", true, Option::STRING }
	};

	...

	OptionManager om;
	om.registerOptions(options, sizeof(options) / sizeof(options[0]));
    </pre>

    Recall that static memory areas are initialized with zeros so that the
    members that are not initialized explicitly in the example above are
    initialized implicitly with zeros (which the OptionManager used to
    determine that they are not used).

    It is possible to specify domains as well. For example, suppose we
    want to define a "color" option that can be in the following set:
    {"red", "green", "blue"}. Here is how to express that:

    <pre>
	static const char* colors[] = { "red", "green", "blue" };
	static const Uint32 NUM_COLORS = sizeof(colors) / sizeof(colors[0]);

	static OptionRow options[] =
	{
	    { "color", "red", false, Option::STRING, colors, NUM_COLORS }
	};
    </pre>
*/
struct OptionRow
{
    const char* optionName;
    const char* defaultValue;
    Boolean required;
    Option::Type type;
    char** domain;
    Uint32 domainSize;
    const char* commandLineOptionName;
};

/** Exception class */
class MissingCommandLineOptionArgument : public Exception
{
public:

    MissingCommandLineOptionArgument(const String& optionName)
	: Exception("Missing command line option argument: " + optionName) { }
};

/** Exception class */
class InvalidOptionValue : public Exception
{
public:

    InvalidOptionValue(const String& name, const String& value)
	: Exception("Invalid option value: " + name + "=\"" + value + "\"") { }
};

/** Exception class */
class DuplicateOption : public Exception
{
public:

    DuplicateOption(const String& name)
	: Exception("Duplicate option: " + name) { }
};

/** Exception class */
class ConfigFileSyntaxError : public Exception
{
public:

    ConfigFileSyntaxError(const String& file, Uint32 line)
	: Exception(_formatMessage(file, line)) { }

    static String _formatMessage(const String& file, Uint32 line);
};

/** Exception class */
class UnrecognizedConfigFileOption : public Exception
{
public:

    UnrecognizedConfigFileOption(const String& name)
	: Exception("Unrecognized config file option: " + name) { }
};

/** Exception class */
class MissingRequiredOptionValue : public Exception
{
public:

    MissingRequiredOptionValue(const String& name)
	: Exception("Missing required option value: " + name) { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_OptionManager_h */

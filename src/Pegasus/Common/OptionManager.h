//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Karl Schopmyer(k.schopmeyer@opengroup.org)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// This file defines the classes necessary to manage commandline and
// configuration file options for Pegasus.  It defines
//   The OptionManager Class
//   The Option Class - Used to define information about an option
//   The Option Row structure - Used to define option declarations in a
//	program
//   The optionexcptions Class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OptionManager_h
#define Pegasus_OptionManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

class Option;
struct OptionRow;

typedef Option* OptionPtr;

// REVIEW: I seem to remember seeing another class that does something like
// REVIEW: this.

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

    Boolean Options can easily be tested as follows:
    <pre>

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

    <h4>Option Types</h4>

    The option manager allows for several types of options including:
    <UL>
	<LI> (BOOLEAN)Simple keyword parameters (ex. -t or -h on the command
	line). These are Boolean parameters and there are no additional parameters
	after the keyword.

	<LI> (INTEGER) Numeric parameters - (ex -port 5988). These are
	parameters where a numeric variable follows the parameter defintion.

	<LI>(WHOLE_NUMBER) Numeric parameters  ATTN: Finish.

	<LI> (NATURAL_NUMBER Numieric parameters - (ex ). ATTN:	finish

	<LI>(STRING) String Parameters - (ex. -file abd.log) These are
	parameters that are represented by strings following the option
	keyword. No limitations are placed on the string except that it must
	be resolvable to a single string.

	<LI> (STRING) Domain Parameters - These are parameters where there is
	a choice of keywords from a domain of keywords.	The input parameter may be any
	one of these keywords. Thus, the domain (red blue green) for the
	parameter "color" (-c) could be entered as -c red. The difference
	between String interpretation and domain interpretation is the use of the
	domain fields in the option definition.

	<LI> Mask parameters - These are parameters that define an internal
	bit mask from a set of keywords input.
	ATTN: Finish this definition.
    </UL>
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
	@exception OMDuplicateOption if option already defined.
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
        &param abortOnErr - Optional Boolean that if true causes exception if there is
        a parameter found that is not in the table. Defaults to true
	&exception InvalidOptionValue if validation fails.
	&exception OMMissingCommandLineOptionArgument
    */
    void mergeCommandLine(int& argc, char**& argv, Boolean abortOnErr=true);

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

	&exception OMMissingRequiredRequiredOption
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
    Boolean lookupValue(const String& name, String& value ) const;

    /** LookupIntegerValue value of an option determines if the value exists
	and converts it to integer (Uint32).
	@param Name provides the name of the option (ex. "port")
	@param String parameter contains the String that contains the
	value for this parameter (in String format).
	@return Boolean return. True if the option found.
    */
    Boolean lookupIntegerValue(const String& name, Uint32& value) const;


    /**	isStringInOptionMask - Looks for a String value in an option.
	This function is used to detect particular options listed in strings of
	entries forming a STRING option.  Thus, for example if the option string
	were "abc,def,ijk" in option toy isStringInOption ("toy", "def") returns
	true.
	@param option  name of the option in the option table
	@param entry  Entry to compare
	@return True if the entry String is found in the option.
    */
    //Uint32 isStringInOptionMask (const String& option, String& entry) const;

    /** optionValueEquals - Test the string value of an option.
	@param name provides the name of the option (ex. "port")
	@param value String value for comparison.
	@return true if the option exists and the value of the option
	equals the input parameter value.
    */
    Boolean valueEquals(const String& name, const String& value) const;

    /** isTrue - determines if a Boolean Option is true or false. Note
        that this function simply tests the value for "true" string.
        @param name - the name of the opeiton
        @return - Returns true if the option value is true.
    */
    Boolean isTrue(const String& name) const;

    /** Print all the options. */
    void print() const;

    /** Print the help line for all options including cmdline name, name
        and the help string
    */
    void printOptionsHelp() const;

    /** Print Complete Help Text message including header before the options help
        and trailer after the options help
    */
    void printOptionsHelpTxt(const String& header, const String& trailer) const;


private:

    /** Lookup the option by its commandLineOptionName.
	@return 0 if no such option.
    */
    Option* _lookupOptionByCommandLineOptionName(const String& name);

    Array<Option*> _options;
};

//////////////////////////////////////////////////////////////////
//    OPTION CLASS
//////////////////////////////////////////////////////////////////

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

	@param optionHelpMessage Text message that defines option. To be used
	    in Usage messages.
    */
    Option(
        const String& optionName,
        const String& defaultValue,
        Boolean required,
        Type type,
        const Array<String>& domain = Array<String>(),
        const String& commandLineOptionName = String(),
        const String& optionHelpMessage = String());

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

    /** Accessor */
    const String& getOptionHelpMessage() const
    {
	return _optionHelpMessage;
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
    String _optionHelpMessage;
    Boolean _resolved;
};

///////////////////////////////////////////////////////////////////////
//  OptionRow
///////////////////////////////////////////////////////////////////'//

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
    When a domain is defined, any of the keywords in that domain are legal
    option keywords.  For example.  With the domain defined above,  a command
    line or config file entry that includes -c blue sets the option "color" to
    blue. Note that this requires a space between -c and blue.
 */
struct OptionRow
{
    const char* optionName;
    const char* defaultValue;
    int required;
    Option::Type type;
    char** domain;
    Uint32 domainSize;
    const char* commandLineOptionName;
    const char* optionHelpMessage;
};
/* NOTE: The "required" object must be an int rather than a Boolean because
    bool on some platforms is not defined so that we cannot use a Boolean here
    with a static object.
*/

/** Exception class */
class OMMissingCommandLineOptionArgument : public Exception
{
public:

	//l10n
    //OMMissingCommandLineOptionArgument(const String& optionName)
	//: Exception("Missing command line option argument: " + optionName) { }
	OMMissingCommandLineOptionArgument(const String& optionName)
	: Exception(MessageLoaderParms("Common.OptionManager.MISSING_CMD_LINE_OPTION",
											 "Missing command line option argument: $0",
											 optionName)) { }
};

/** Exception class */
class OMInvalidOptionValue : public Exception
{
public:

	//l10n
    //OMInvalidOptionValue(const String& name, const String& value)
	//: Exception("Invalid option value: " + name + "=\"" + value + "\"") { }
	OMInvalidOptionValue(const String& name, const String& value)
	: Exception(MessageLoaderParms("Common.OptionManager.INVALID_OPTION_VALUE",
											 "Invalid option value: $0=\"$1\"",
											 name,
											 value)) { }
};

/** Exception class */
class OMDuplicateOption : public Exception
{
public:
//l10n
    //OMDuplicateOption(const String& name)
	//: Exception("Duplicate option: " + name) { }
	OMDuplicateOption(const String& name)
	: Exception(MessageLoaderParms("Common.OptionManager.DUPLICATE_OPTION",
											 "Duplicate option: $0",
											 name)) { }
};

/** Exception class */
class OMConfigFileSyntaxError : public Exception
{
public:

    OMConfigFileSyntaxError(const String& file, Uint32 line)
	: Exception(_formatMessage(file, line)) { }

    static String _formatMessage(const String& file, Uint32 line);
};

/** Exception class */
class OMUnrecognizedConfigFileOption : public Exception
{
public:
//l10n
    //OMUnrecognizedConfigFileOption(const String& name)
	//: Exception("Unrecognized config file option: " + name) { }
	OMUnrecognizedConfigFileOption(const String& name)
	: Exception(MessageLoaderParms("Common.OptionManager.UNRECOGNIZED_CONFIG_FILE_OPTION",
											 "Unrecognized config file option: $0",
											 name)) { }
};

/** Exception class */
class OMMissingRequiredOptionValue : public Exception
{
public:
//l10n
    //OMMissingRequiredOptionValue(const String& name)
	//: Exception("Missing required option value: " + name) { }
	OMMissingRequiredOptionValue(const String& name)
	: Exception(MessageLoaderParms("Common.OptionManager.MISSING_REQUIRED_OPTION",
											 "Missing required option value: $0",
											 name)) { }
};

/** Exception class */
class OMMBadCmdLineOption : public Exception
{
public:
//l10n
   // OMMBadCmdLineOption(const String& name)
	//: Exception("Parameter not Valid: " + name) { }
	OMMBadCmdLineOption(const String& name)
	: Exception(MessageLoaderParms("Common.OptionManager.PARAMETER_NOT_VALID",
											 "Parameter not Valid: $0",
											 name)) { }
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_OM_h */

//
// Yet another attempt to create a general-purpose, object-oriented,
// portable C++ command line parser.
//
// There are two classes involved:
//    getoopt which encapsulates three functions:
//        1. Registration of program-defined flags and their characteristics
//        2. Parsing of the command line IAW the flag definitions
//        3. Orderly retrieval of the command line components
//
//    Optarg which abstracts the idea of a command line argument.
//
// The details of these classes are discussed in the comments above each
// class.
// 

#ifndef _GETOOPT_H_
#define _GETOOPT_H_

#include <string>
#include <vector>
#include <iostream>
#include <Pegasus/Common/Config.h>

using namespace std;

//
// This structure describes a program-defined command line option.
// The syntax of these options are the same as those understood by
// the standard C language routines getopt() and getopt_long() 
//
// Command line options are named, and the name is prefaced by
// either a hyphen or by two hyphens.  Names prefixed by one
// hyphen are restricted to a length of one character.  There is
// no limit to the size of names prefixed by two hyphens.  The
// two-hyphen-named options are called "long" options.
//
// The argtype indicates whether the name should be bound to a value.
// If it never has a value, the type is 0.  If it always has a value,
// the type is 1.  If it can optionally have a value, the type is 2.
// Type 2 is valid only with long-named options
//
// The islong flag tells whether the option is long-named.
// 
// The isactive flag tells whether the option should be considered
// during parsing.  It is on unless explicitly turned off by the program.
struct flagspec {
  string name;
  int    argtype;
  bool   islong;
  bool   active;
};

//
// Class Optarg encapsulates a command line argument as it was parsed.
// If it has a name, it means that it is bound to that command line option.
// For example, if the command line were
//      myprog --database=xyz mytable
// then the name "database" would be associated with the first argument.
// There would be no name associated with the second argument ("mytable").
//
// In the example above, the value property of the arguments would
// be "xyz" bound to the name "database" and "mytable" bound to a
// blank name.
//
// The option type further describes the binding:
//   A FLAG means that the value is bound to a short-named option name (flag)
//   A LONGFLAG means that the value is bound to a long-named option name
//   REGULAR means that the argument value is not preceded by a flag
//
class PEGASUS_GETOOPT_LINKAGE Optarg {
 public:
  enum opttype {FLAG, LONGFLAG, REGULAR};
 private:
  string _name;
  opttype _opttype;
  string _value;
 public:
  // Constructors and Destructor.  Default copying is OK for this class.
  Optarg();
  Optarg(const string &name, opttype type, const string &value);
  ~Optarg();
  // Methods to set or reset the properties
  void setName(const string &name);
  void setType(opttype type);
  void setValue(const string &value);
  // Methods to get information about the object
  const string &getName() const;
  const string &getopt() const;
  opttype getType() const;
  bool  isFlag() const;  // Is the opttype == "FLAG" or "LONGFLAG"?
  bool isLongFlag() const;  // IS the Opttype == LONGFLAG?
  const string &Value() const;  // return the value as a string
  const string &optarg() const; // ditto, in getopt() terminology
  void Value(string &v) const ; // Fill in a string with the Value
  void Value(int &v) const ;    // Fill in an int with the value
  void Value(unsigned int &v) const;  // ditto an unsigned int
  void Value(long &v) const ;   // ditto a long
  void Value(unsigned long &v) const;  // ditto an unsigned long
  void Value(double &d) const;  // ditto a double
  ostream &print(ostream &os) const;  // print the members (for debug)
};

//
//  class getoopt (a portamentau of "getopt" and "oo") is a container
//  for Optarg objects parsed from the command line, and it provides
//  methods for specifying command line options and initiating the
//  parse.
//
//  The idea is to be able to do getopt()-like things with it:
//      getoopt cmdline(optstring);
//      for (getoopt::const_iterator it = cmdline.begin();
//      	it != cmdline.end();
//      	    it++) {
//        . . . (process an Optarg represented by *it.
//
//  There are three steps in using this class:
//    1. Initialization -- specifying the command line options
//       You can pass a string identical in format the an optstring
//       to the object either in the constructor or by an explicit method.
//       If you have long-named options to describe, use as manu
//       addLongFilespec() calls as you need.
//    2. Parse the command line.
//       This will almost always be cmdline.parse(argc, argv);
//       You can check for errors (violations of the command line
//       options specified) by calling hasErrors().  If you need
//       a description of the errors, they are stored in a
//       vector<string> which can be retrieved with getErrorStrings();
//       You can also print the error strings with printErrors();
//    3. Analyze the parsed data.  You can either iterate through the
//       the command line, or use indexes like this:
//       for (unsigned int i = cmdline.first(); i < cmdline.last(); i++)
//         . . . (process an Optarg represented by cmdline[i])
//
//       You can also look at the parsed data for named arguments
//       in an adhoc fashion by calling
//           isSet(flagName);
//       and
//           value(flagName);
//           
class PEGASUS_GETOOPT_LINKAGE getoopt {
 public:
  typedef vector<flagspec> Flagspec_List;
  typedef vector<string>   Error_List;
  typedef vector<Optarg>   Arg_List;
 private:
  Flagspec_List  _flagspecs;
  Error_List     _errorStrings;
  Arg_List       _args;
  flagspec *getFlagspecForUpdate(char c);
  flagspec *getFlagspecForUpdate(const string &s);
  string emptystring;
  Optarg _emptyopt;
 public:
  enum argtype {NOARG, MUSTHAVE, OPTIONAL};
  // Constructor and destructor.  You can initialize an instance with
  // an optstring to specify command line flags.
  getoopt(const char *optstring = 0);
  ~getoopt();
  
  // Routines for specifying the command line options
  //   add short-named flags, either en masse as an optstring
  bool addFlagspec(const string &opt);
  //   or individually
  bool addFlagspec(char opt, bool hasarg = false);
  //   (You can also remove a short flag specification if you need to)
  bool removeFlagspec(char opt);
  //   You can add long-named flags only individually
  bool addLongFlagspec(const string &name,  argtype type);
  //   and remove them in the same way.
  bool removeLongFlagspec(const string &name);
  // You can also get a pointer to the flagspec structure for
  // a particular flag, specifying a char for short or string for long name
  const flagspec *getFlagspec(char c);
  const flagspec *getFlagspec(const string &s);

  // Routines for initiating the parse and checking its success.
  bool parse(int argc, char **argv);
  bool hasErrors() const;
  const Error_List &getErrorStrings() const;
  ostream &printErrors(ostream &os) const;

  // Routines for processing the parsed command line
  //   Using iterators
  typedef Arg_List::const_iterator const_iterator;
  Optarg *begin();
  Optarg *end();
  //   Using indexes
  unsigned int size() const;  // The number of arguments found
  const Optarg &operator[](unsigned int n);  // The nth element
  unsigned int first() const;  // always 0 (duh)
  unsigned int last() const;   // always == size();
  //   Ad Hoc
  //        isSet returns the number of times a particular option appeared
  //        in the argument set.
  unsigned int  isSet(char opt) const;
  unsigned int  isSet(const string &opt) const;
  //        value returns the string value bount to the nth instance of
  //        the flag on the command line
  const string &value(char opt, unsigned int idx = 0) const;
  const string &value(const string &opt, unsigned int idx = 0) const;
  //   Still not flexible enough?  Here's an array of the results for
  //   your perusal.
  const Arg_List &getArgs() const;

  // Miscellanous methods
  //   You can add your own error to the error list if you want
  void  addError(const string &errstr);
  //   This method gives the number of named arguments (flags)
  //   size() - flagent() == number of nonflag arguments.
  unsigned int flagcnt() const;
};


#endif

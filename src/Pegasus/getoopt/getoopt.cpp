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
// Author: Bob Blair bblair@bmc.com
//
// $Log: 
// 
//
//END_HISTORY
//
// implementation of getoopt

#include "getoopt.h"
#include <string.h>

using namespace std;

//-----------------------------------------------------------------------
//              Implementation of class Optarg
// An Optarg is created for each parameter on the command line.
//-----------------------------------------------------------------------

// Constructors
//   Default Constructor
Optarg::Optarg() : _name(""), _opttype(REGULAR), _value("") {
}

//   All-in-one
Optarg::Optarg(const string &name, opttype type, const string &value) :
  _name(name), _opttype(type), _value(value) {
}

// Destructor
Optarg::~Optarg() {};

//-----------------------------------------------------------------------
//        Set the class members
//-----------------------------------------------------------------------

// Set the _name member
void
Optarg::setName(const string &name) {
  _name = name;
}

// Set the _opttype member
void
Optarg::setType(opttype type) {
  _opttype = type;
}

// Set the _value member
void
Optarg::setValue(const string &value) {
  _value = value;
}

//-----------------------------------------------------------------------
//      Retrieve class members
//-----------------------------------------------------------------------

//  Get the _name member
const string &
Optarg::getName() const { return _name; }

//  Get the _name member using getopt() terminology
const string &
Optarg::getopt()  const { return _name; }

//  Get the _type member
Optarg::opttype 
Optarg::getType() const { return _opttype; }

//-------------------------------------------------------------------
//             Ways to get the _value member
//-------------------------------------------------------------------

//  Return _value as const string ref
const string &
Optarg::Value() const { return _value; }

//  Same thing as Value(), but using getopt() terminology
const string &
Optarg::optarg() const { return _value; }

//  Fill in a caller-provided string
void
Optarg::Value(string &s) const { s = _value; }

//  Fill in a caller-provided int with the integer conversion of the value.
void
Optarg::Value(int &i) const { i = (int)atoi(_value.c_str()); }

//  Fill in a caller-provided unsigned int
void
Optarg::Value(unsigned int &i) const {
  i = (unsigned int)atoi(_value.c_str());
}

//  Fill in a call-provided unsigned int
void
Optarg::Value(long &l) const {
  l = (long)atoi(_value.c_str());
}

//  Fill in a caller-provided long
void
Optarg::Value(unsigned long &l) const {
  l = (unsigned long)atoi(_value.c_str());
}

//  Ditto unsigned long
void
Optarg::Value(double &d) const {
  d = (double)atof(_value.c_str());
}

//--------------------------------------------------------------------
//  Provide information about the flag, if any
//--------------------------------------------------------------------

// Is the option value is bound to a flag?
bool
Optarg::isFlag() const { return (_opttype == FLAG || _opttype == LONGFLAG); }

// Is it bound to a long-named flag?
bool
Optarg::isLongFlag() const { return (_opttype == LONGFLAG); }

//-----------------------------------------------------------------------
//  print the members as a formatted string
//-----------------------------------------------------------------------
ostream &
Optarg::print(ostream &os) const {
  os << "{name:(" << getName();
  os << ") type:(";
  switch (getType()) {
  case FLAG: os << "FLAG"; break;
  case LONGFLAG: os << "LONGFLAG"; break;
  case REGULAR: os << "REGULAR"; break;
  }
  os << ") value:(" << Value() << ")}";
  return os;
}



//---------------------------------------------------------------------
//                   Implementation of class getoopt
//---------------------------------------------------------------------

// Constructors and destructor

// Default constructor.  The optional string is in the format of
// a getopt() optstring 
getoopt::getoopt(const char *optstring)
{
  if (optstring) {
    addFlagspec(optstring);
  }
}

getoopt::~getoopt() {;}

//----------------------------------------------------------------------
//  methods to register program-defined flags and their characteristics
//  The flags are encapsulated in the struct flagspec, an array of which
//  is a member of this class.
//  There are also methods to deregister flags. This is done by resetting
//  a flagspec's active flag.
//----------------------------------------------------------------------

// Parse through a getopt() optstring and create flagspecs from each
// short flag.
bool
getoopt::addFlagspec(const string &opt) {
  unsigned int size = opt.size();
  if (size == 0)
    return false;
  for (unsigned int i = 0; i < size; i++) {
    char c = opt[i];
    if ( ((i + 1) < size) && (opt[i+1] == ':') ) {
      if (!(addFlagspec(c, true))) {
	  return false;
      }
      ++i;
    } else {
      if (!(addFlagspec(c, false)))
	return false;
    }
  }
  return true;
}

//  Create a filespec from a single short flag and push it onto the array
bool
getoopt::addFlagspec(char flag, bool hasarg) {
  if (flag == '*') {
    addError("You can't have a flag named '*'");
    return false;
  }
  flagspec fs;
  char c[2];
  c[0] = flag;
  c[1] = 0;
  fs.name = c;
  fs.argtype = hasarg ? 1 : 0;
  fs.islong = false;
  fs.active = true;
  _flagspecs.push_back(fs);
  return true;
}

// Create a flagspec from a single long flag and push it onto the array
bool
getoopt::addLongFlagspec(const string &name, argtype type) {
  flagspec fs;
  fs.name = name;
  fs.argtype = type;
  fs.islong = true;
  fs.active = true;
  _flagspecs.push_back(fs);
  return true;
}

// Unregister a flagspec
bool
getoopt::removeFlagspec(char opt) {
  flagspec *fs = getFlagspecForUpdate(opt);
  if (fs) {
    fs->active = false;
    return true;
  }
  return false;
}

//--------------------------------------------------------------------
//      Routines for parsing the command line
//--------------------------------------------------------------------

//------------------------
// Static functions
//------------------------

// Parse out the flagname and the value from a long flag option that
// may be in the form
//          --longflag=value
static void
partsFromLongOpt (const string &s, string &name, string &value) {
  for (unsigned int i = 0; i < s.size(); i++) {
    if (s[i] == '=') {
      name = s.substr(0, i);
      value = s.substr(i+1);
      return;
    }
  }
  name = s;
  value =  "";
}

// Create an Optarg instance from a long flag string like
//          --longflag=value
// (The =value is optional).
static void 
optargFromLongOpt(Optarg &o, const string &arg) {
  string name;
  string value;
  partsFromLongOpt(arg, name, value);
  o.setName(name);
  o.setType(Optarg::LONGFLAG);
  o.setValue(value);
}

// Create an Optarg instance from a short flag string like
//      -fValue
// (The Value part is optional)
static void
optargFromShortOpt(Optarg &o, const char *arg) {
  char name[2];
  name[0] = arg[0];
  name[1] = 0;
  o.setName(name);
  o.setType(Optarg::FLAG);
  const char *p = arg + 1;
  o.setValue(p);
}

// Look at a command line option and determine whether it is a
// long flag, a short flag or an unflagged option.
static int
catagorize(const char *s) {
  if (s[0] != '-')
    return 0;
  else
    if (s[1] == '-')
      return 2;
  return 1;
}

// Push an Optarg onto our array
static void
addarg(getoopt::Arg_List&list, const Optarg &o) {
  //o.print(cout);
  list.push_back(o);
}

// Create an Optarg from its members and push it onto the array
static void
addarg(getoopt::Arg_List&list, const string &name, Optarg::opttype type,
	      const string &value) {
  Optarg *o = new Optarg(name, type, value);
  addarg(list, *o);
  delete o;
}

// Take an array of arguments and append it to another
static void
copyargs(getoopt::Arg_List &out, const getoopt::Arg_List &in) {
  for (getoopt::Arg_List::const_iterator it = in.begin(); 
       it != in.end();
       it++) {
    addarg(out, *it);
  }
}

//------------------------------------
// The parse method:  Way too long.
// Note that flag args are pushed
// onto the stack, then the regular
// args are appended, sorting them
// to the rear the way getopt() does.
//------------------------------------
bool
getoopt::parse(int argc, char **argv) {
  Optarg o;
  int cat;
  const flagspec *fs;
  Arg_List nonflagargs;
  enum states {START, ARGEXPECTED};
  states state = START;
  for (unsigned int i = 1; i < (unsigned int)argc; i++) {
    unsigned int endsize = strlen(argv[i]);
      switch (state) {
      case START:
        cat = catagorize(argv[i]);
	switch (cat) {
	case 0: // non-flag command line argument
	  addarg(nonflagargs, "", Optarg::REGULAR, argv[i]);
	  break;
	case 1: // short (1-character) flag
	  {
	    unsigned int argpos = 1;
	    while (argpos < endsize) {
	      char c = argv[i][argpos];
	      fs = getFlagspec(c);  // Short flag
	      string temp = argv[i];
	      string name = temp.substr(argpos, 1);
	      if (!fs) {  // See if we recognize it
		addError("Unknown flag -" + name);
		argpos++;
	      } else {
		if (fs->argtype == NOARG) {  // Should this flag be bound
		  addarg(_args, name, Optarg::FLAG,  "");  // NO
		  argpos++;
		} else { // YES -- the value is here or in the next arg
		  optargFromShortOpt(o, &argv[i][argpos]);
		  if (o.Value() == "") { // No value yet
		    state = ARGEXPECTED;
		  } else {
		    addarg(_args, o);
		  }
		  argpos = endsize;
		}
	      }
	    }
	  } // end subcase 1
	  break;
	case 2:  // long (--xyz) flag
	  { 
	    string arg = &(argv[i][2]);
	    optargFromLongOpt(o, arg);
	    fs = getFlagspec(o.getName());
	    if (!fs) { // see if we recognize this flag
	      string temp = "Unknown flag ";
	      addError(temp + o.getName());
	    } else {
	        // this is a long flag we know about
	      if (o.optarg() != ""  || fs->argtype != MUSTHAVE) { 
		addarg(_args, o);
		state = START;  // we have a completed long flag
	      } else {   // no value yet, and we expect one
		if (fs->argtype == MUSTHAVE) {
		  state = ARGEXPECTED;
		}
	      }
	    }
	    break;
	  } // end subcase 2
	} // end switch catagorize()
	break; // end of case START

      case ARGEXPECTED:
	if (argv[i][0] == '-') {
	  addError("Missing required value for flag " + o.getopt());
	  i--;
	} else {
	  o.setValue(argv[i]);
	}
	addarg(_args, o);
	state = START;
	break;
      } // end switch
  } // end for
  if (state != START) {
    addError("Missing required value for flag " + o.getName());
  }
  copyargs(_args, nonflagargs);
  return !_errorStrings.size();
}

//----------------------------------------------------------------------
//         Methods to retrieve the command line arguments
//----------------------------------------------------------------------

//----------------------------------------------
// Access the command line arguments by index
//----------------------------------------------

// Index operator
const Optarg &
getoopt::operator[](unsigned int n) {
  unsigned int lim = _args.size();
  if (n < lim) 
    return _args[n];
  else
    return _emptyopt;
}

// Return first index
unsigned int
getoopt::first() const { return 0; }

// Return one past last index
unsigned int
getoopt::last() const { return _args.size(); }

//-----------------------------------------------
// Access the command line arguments using
// getoopt::const_iterator
//-----------------------------------------------

Optarg *
getoopt::begin() {
  return _args.begin();
}

Optarg *
getoopt::end() {
  return _args.end();
}

//-----------------------------------------------
// Access the command line arguments ad-hoc
//-----------------------------------------------

// Return the number of times a short flag is set
// on the command line
unsigned int
getoopt::isSet(char c) const {
  unsigned int cnt = 0;
  for (unsigned int i = 0; i < _args.size(); i++) {
    const Optarg &o = _args[i];
    if (o.getType() == Optarg::FLAG) {
      const string &s = o.getopt();
      if (s[0] == c) {
	cnt++;
      }
    }
  }
  return cnt;
}

// Return the number of times any flag is set
// on the command line
unsigned int
getoopt::isSet(const string &s) const {
  unsigned int cnt = 0;
  for (unsigned int i = 0; i < _args.size(); i++) {
    const Optarg &o = _args[i];
    if (o.optarg() == s) {
      cnt++;
    }
  }
  return cnt;
}

// Return the string value of the nth instance of
// a particular short flag on the command line
const string &
getoopt::value(char opt, unsigned int idx) const {
  unsigned int cnt = 0;
  for (unsigned int i = 0; i < _args.size(); i++) {
    const Optarg &o = _args[i];
    if (o.getType() == Optarg::FLAG) {
      const string &s = o.getopt();
      if (s[0] == opt) {
	if (cnt == idx) {
	  return o.optarg();
	} else {
	  cnt++;
	}
      }
    }
  }
  return(emptystring);
}

// Return the nth instance of any flag on the command line
const string &
getoopt::value(const string &opt, unsigned int idx) const {
  unsigned int cnt = 0;
  for (unsigned int i = 0; i < _args.size(); i++) {
    const Optarg &o = _args[i];
    if (o.optarg() == opt) {
      if (cnt == idx) {
	return o.getopt();
      } else {
	cnt++;
      }
    }
  }
  return(emptystring);
}

// Of the command line arguments, how many are flags?
unsigned int
getoopt::flagcnt() const {
  unsigned int cnt = 0;
  for (Arg_List::const_iterator it = _args.begin();
       it != _args.end();
       it++) {
    if (it->getType() != Optarg::REGULAR)
      cnt++; 
  }
  return cnt;
}

// How many command line arguments were there?
unsigned int
getoopt::size() const {
  return _args.size();
}

// Return the list of command line arguments for use by
// the program.
const getoopt::Arg_List &
getoopt::getArgs() const { return _args; }


//-----------------------------------------------------------
// Routines dealing with errors during parsing
// FIXME:  This needs to be reworked so that the error text
// is hidden and provided by the caller
//----------------------------------------------------------

// Add an error into the list
void
getoopt::addError(const string &s)
{
  _errorStrings.push_back(s);
}

// Return a list of the errors
const getoopt::Error_List &
getoopt::getErrorStrings() const {
  return _errorStrings;
}

// Did any errors occur?
bool
getoopt::hasErrors() const {
  return _errorStrings.size() ? true : false;
}



flagspec *
getoopt::getFlagspecForUpdate(const string &s) {
  for (unsigned int i = 0; i < _flagspecs.size(); i++) {
    flagspec &o = _flagspecs[i];
    if (o.islong && s == o.name)
      return &_flagspecs[i];
  }
return 0;
}

const flagspec *
getoopt::getFlagspec(const string &s) {
  return (const flagspec *)getFlagspecForUpdate(s);
}

ostream &
getoopt::printErrors(ostream &os) const {
  for (Error_List::const_iterator it = _errorStrings.begin();
       it != _errorStrings.end();
       it++) {
    os << "> " << *it << endl;
  }
  return os;
}

//---------------------------------------------------------------
//              Private methods
//---------------------------------------------------------------
flagspec *
getoopt::getFlagspecForUpdate(char c) {
  for (unsigned int i = 0; i < _flagspecs.size(); i++) {
    flagspec &o = _flagspecs[i];
    if (!o.islong && c == o.name[0])
      return &_flagspecs[i];
  }
  return 0;
}

const flagspec *
getoopt::getFlagspec(char c) {
  return (const flagspec *)getFlagspecForUpdate(c);
}

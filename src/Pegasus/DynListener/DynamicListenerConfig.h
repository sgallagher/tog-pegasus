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
// Author: Heather Sterling (hsterl@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DynamicListenerConfig_h
#define Pegasus_DynamicListenerConfig_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/DynListener/Linkage.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

/* This is basically a wrapper for the OptionManager that does an additional level of validation specific to the
 * application.  
 */ 

class PEGASUS_DYNLISTENER_LINKAGE DynamicListenerConfig
{
public:
    
    DynamicListenerConfig();
    
    ~DynamicListenerConfig();

    void initOptions(const String& configFile);

    Boolean lookupValue(const String& name, String& value ) const;

    Boolean lookupIntegerValue(const String& name, Uint32& value) const;

    Boolean valueEquals(const String& name, const String& value) const;

    Boolean isTrue(const String& name) const;

    static DynamicListenerConfig* getInstance();
 
    static void setListenerHome(const String& home);

    static String getListenerHome();

    static String getHomedPath(const String& value);

private:

    static DynamicListenerConfig* _instance;

    static String _listenerHome;

    OptionManager _optionMgr;

    String _configFile;


};

PEGASUS_NAMESPACE_END

#endif //Pegasus_DynamicListenerConfig


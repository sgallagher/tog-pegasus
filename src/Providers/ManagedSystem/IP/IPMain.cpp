//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%////////////////////////////////////////////////////////////////////////////

#include "IPPEpProvider.h"
#include "BIPTLEpProvider.h"
#include "IPRouteProvider.h"
#include "NextHopIPRouteProvider.h"
#include "RSApProvider.h"
#include "RUEpProvider.h"
#include "ANHProvider.h"

//
// Required to build this provider from SDK
//
#if !defined (PEGASUS_OS_VMS)
# include <Pegasus/Common/PegasusVersion.h>
#endif

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


extern "C" PEGASUS_EXPORT CIMProvider* PegasusCreateProvider(String &name)
{
    if (name == "IPPEpProvider")
        return new IPPEpProvider;
    else if (name == "BIPTLEpProvider")
        return new BIPTLEpProvider;
    else if (name == "IPRouteProvider")
        return new IPRouteProvider;
    else 
    {
        if (name == "NextHopIPRouteProvider")
        {
            return new NextHopIPRouteProvider;
        }
        else 
        {
            if (name == "RSApProvider")
            {
                return new RSApProvider;
            }
            else
            { 
                if (name == "RUEpProvider")
                {
                    return new RUEpProvider;
                }
                else
                {
                    if (name == "ANHProvider")
                    {
                        return new ANHProvider;
                    }
                    else
                    { 
                        return 0;
                    }
                }
            }
        }
    }
}


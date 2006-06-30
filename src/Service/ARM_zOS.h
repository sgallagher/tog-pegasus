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
// Author: Thilo Boehm (tboehm@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef ARM_zOS_h
#define ARM_zOS_h

// #include <Pegasus/Common/Config.h>                    

PEGASUS_NAMESPACE_BEGIN

// Register status of CIM Server with ARM
enum __arm_status_tags {
    NOT_REGISTERED,
    REGISTERED,
    RESTARTED
};


class ARM_zOS {

private:
    __arm_status_tags ARM_zOS_Status;

public:
    // constructor
    ARM_zOS(){
        ARM_zOS_Status =  NOT_REGISTERED;
    };

    // destructor
    ~ARM_zOS(){};

    // Register CIM Server with ARM
    void Register(void);
    // De-Register CIM Server from ARM
    void DeRegister(void);

    // get register status
    __arm_status_tags getStatus(void){
        return ARM_zOS_Status;
    };

}; //class ARM_zOS

PEGASUS_NAMESPACE_END

#endif // ARM_zOS_h

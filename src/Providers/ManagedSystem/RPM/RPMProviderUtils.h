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
//=============================================================================
//
// Author: Tim Potter <tpot@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef _RPMPROVIDERUTILS_H
#define _RPMPROVIDERUTILS_H

#include <Pegasus/Common/CIMInstance.h>
#include "RPMWrapper.h"

// Include rpm C library headers

extern "C" {
#include <rpm/rpmdb.h>
#include <rpm/rpmmacro.h>
}

// Constants

extern const String PackageClassName;
extern const String FileClassName;
extern const String ProductClassName;

enum SoftwareElementState {
    Deployable, Installable, Executable, Running
};

enum OSType {
    Unknown, Other, MACOS, ATTUNIX, DGUX, DECNT, Digital_Unix,
    OpenVMS, HP_UX, AIX, MVS, OS400, OS2, JavaVM, MSDOS, WIN3x,
    WIN95, WIN98, WINNT, WINCE, NCR3000, NetWare, OSF,
    DCOS, Reliant_UNIX, SCO_UnixWare, SCO_OpenServer, Sequent,
    IRIX, Solaris, SunOS, U6000, ASERIES, TandemNSK, TandemNT,
    BS2000, LINUX, Lynx, XENIX, VM_ESA, Interactive_UNIX,
    BSDUNIX, FreeBSD, NetBSD, GNU_Hurd, OS9, MACH_Kernel,
    Inferno, QNX, EPOC, IxWorks, VxWorks, MiNT, BeOS, HP_MPE,
    NextStep, PalmPilot, Rhapsody, Windows_2000, Dedicated,
    OS_390, VSE, TPF, Windows_Me, Open_UNIX, OpenBDS, NotApplicable,
    Windows_XP, zOS, Windows_2003, Windows_2003_64
};

//
// Helper functions
//

// Misc

Uint16 getTargetOperatingSystem();

// Convert file headers to CIMObjectPath or CIMInstance objects

CIMObjectPath packageObjectPathFromHeader(Header h);
CIMInstance packageInstanceFromHeader(Header h);

CIMObjectPath packageReferenceNameFromHeader(Header h, 
					     const CIMObjectPath &file);
CIMInstance packageReferenceFromHeader(Header h, const CIMObjectPath &file);

CIMObjectPath productObjectPathFromHeader(Header h);
CIMInstance productInstanceFromHeader(Header h);

// Convert package headers to arrays of CIMObjectpath or CIMInstance
// objects

Array<CIMObjectPath> fileObjectPathsFromHeader(Header h);
Array<CIMInstance> fileInstancesFromHeader(Header h);

Array<CIMObjectPath> fileReferenceNamesFromHeader(Header h);
Array<CIMInstance> fileReferencesFromHeader(Header h);

#endif // _RPMPROVIDERUTILS_H

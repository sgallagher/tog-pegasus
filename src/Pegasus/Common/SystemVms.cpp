//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <descrip.h>           //  $DESCRIPTOR
#include <iodef.h>             // IO$_SENSEMODE
#include <ttdef.h>             // TT$M_NOBRDCST
#include <tt2def.h>            // TT2$M_PASTHRU
#include <starlet.h>
#include <ssdef.h>             // SS$_NORMAL
#include <stsdef.h>            // VMS_STATUS_SUCCESS
#include <prvdef>              // PRV$M_SETPRV

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// System
//
//==============================================================================

String System::getPassword(const char* prompt)
{
    struct
    {
        short int numbuf;
        char frst_char;
        char rsv1;
        long rsv2;
    }
    tahead;

    typedef struct
    {                           // I/O status block
        short i_cond;           // Condition value
        short i_xfer;           // Transfer count
        long i_info;            // Device information
    }
    iosb;

    typedef struct
    {                           // Terminal characteristics
        char t_class;           // Terminal class
        char t_type;            // Terminal type
        short t_width;          // Terminal width in characters
        long t_mandl;           // Terminal's mode and length
        long t_extend;          // Extended terminal characteristics
    }
    termb;

    termb otermb;
    termb ntermb;

    short int ichan;            // Gets channel number for TT:

    int errorcode;
    int kbdflgs;                // saved keyboard fd flags
    int kbdpoll;                // in O_NDELAY mode
    int kbdqp = false;          // there is a char in kbdq
    int psize;                  // size of the prompt

    const size_t MAX_PASS_LEN = 32;
    static char buf[MAX_PASS_LEN];
    char kbdq;                  // char we've already read

    iosb iostatus;

    static long termset[2] = { 0, 0 };  // No terminator

    $DESCRIPTOR(inpdev, "TT");  // Terminal to use for input

    buf[0] = 0;

    ichan = 0;

    try
    {
        // Get a channel for the terminal

        errorcode = sys$assign(&inpdev,     // Device name
                               &ichan,      // Channel assigned
                               0,   // request KERNEL mode access
                               0);  // No mailbox assigned

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$assign failure");
        }

        // Read current terminal settings

        errorcode = sys$qiow(0,     // Wait on event flag zero
                             ichan, // Channel to input terminal
                             IO$_SENSEMODE, // Function - Sense Mode
                             &iostatus,     // Status after operation
                             0, 0,  // No AST service
                             &otermb,       // [P1] Address of Char Buffer
                             sizeof (otermb),       // [P2] Size of Char Buffer
                             0, 0, 0, 0);   // [P3] - [P6]

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_SENSEMODE failure");
        }

        // setup new settings

        ntermb = otermb;

        // turn on passthru and nobroadcast

        ntermb.t_extend |= TT2$M_PASTHRU;
        ntermb.t_mandl |= TT$M_NOBRDCST;

        // Write out new terminal settings

        errorcode = sys$qiow(0,     // Wait on event flag zero
                             ichan, // Channel to input terminal
                             IO$_SETMODE,   // Function - Set Mode
                             &iostatus,     // Status after operation
                             0, 0,  // No AST service
                             &ntermb,       // [P1] Address of Char Buffer
                             sizeof (ntermb),       // [P2] Size of Char Buffer
                             0, 0, 0, 0);   // [P3] - [P6]

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_SETMODE failure");
        }

        // Write a prompt, read characters from the terminal, performing no
        // editing
        // and doing no echo at all.

        psize = strlen(prompt);

        errorcode = sys$qiow(0,     // Event flag
                             ichan, // Input channel
                             IO$_READPROMPT | IO$M_NOECHO | IO$M_NOFILTR |
                             IO$M_TRMNOECHO,
                             // Read with prompt, no echo, no translate, no
                             // termination character echo
                             &iostatus,     // I/O status block
                             NULL,  // AST block (none)
                             0,     // AST parameter
                             &buf,  // P1 - input buffer
                             MAX_PASS_LEN,  // P2 - buffer length
                             0,     // P3 - ignored (timeout)
                             0,     // P4 - ignored (terminator char set)
                             prompt,        // P5 - prompt buffer
                             psize);        // P6 - prompt size

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_READPROMPT failure:");
        }

        // Write out old terminal settings
        errorcode = sys$qiow(0,     // Wait on event flag zero
                             ichan, // Channel to input terminal
                             IO$_SETMODE,   // Function - Set Mode
                             &iostatus,     // Status after operation
                             0, 0,  // No AST service
                             &otermb,       // [P1] Address of Char Buffer
                             sizeof (otermb),       // [P2] Size of Char Buffer
                             0, 0, 0, 0);   // [P3] - [P6]

        if (errorcode != SS$_NORMAL)
        {
            throw Exception("sys$qiow IO$_SETMODE failure");
        }

        // Start new line

        const int CR = 0x0d;
        const int LF = 0x0a;
        fputc(CR, stdout);
        fputc(LF, stdout);

        // Remove the termination character
        psize = strlen(buf);
        buf[psize - 1] = 0;
    }

    catch (Exception &e)
    {
        PEG_TRACE((TRC_OS_ABSTRACTION, Tracer::LEVEL1, "%s: %s",
            (const char *) (e.getMessage()).getCString(),
            strerror(EVMSERR, errorcode)));
    }

    // Deassign the channel

    if (ichan) sys$dassgn(ichan);

    return buf;
}

String System::encryptPassword(const char* password, const char* salt)
{
    const size_t MAX_PASS_LEN = 1024;
    char pbBuffer[MAX_PASS_LEN] = {0};
    Uint32 dwByteCount;
    char pcSalt[3] = {0};

    strncpy(pcSalt, salt, 2);
    dwByteCount = strlen(password);
    memcpy(pbBuffer, password, dwByteCount);

    for (Uint32 i=0; (i<dwByteCount) || (i>=MAX_PASS_LEN); i++)
    {
        (i%2 == 0) ? pbBuffer[i] ^= pcSalt[1] : pbBuffer[i] ^= pcSalt[0];
    }

    return String(pcSalt) + String((char *)pbBuffer);
}

Boolean System::isPrivilegedUser(const String& userName)
{
    static union prvdef old_priv_mask;
    static union prvdef new_priv_mask;
    char enbflg = 1; // 1 = enable
    char prmflg = 0; // 0 = life time of image only.
    int retStat;

    old_priv_mask.prv$v_sysprv = false;    // SYSPRV privilege.
    new_priv_mask.prv$v_sysprv = true;     // SYSPRV privilege.

    retStat = sys$setprv(enbflg, &new_priv_mask, prmflg, &old_priv_mask);
    if (!$VMS_STATUS_SUCCESS(retStat))
    {
        return false;
    }

    if (retStat == SS$_NOTALLPRIV)
    {
        return false;
    }

    return true;
}

PEGASUS_NAMESPACE_END

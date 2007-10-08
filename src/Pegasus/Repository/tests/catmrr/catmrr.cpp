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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MofWriter.h>
#include <Pegasus/Repository/Serialization.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    // Usage:

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s path\n", argv[0]);
        exit(1);
    }

    // Open input file:

    FILE* is = fopen(argv[1], "rb");

    if (!is)
    {
        fprintf(stderr, "%s: failed to open \"%s\"\n", argv[0], argv[1]);
        exit(1);
    }

    // Read file into memory:

    Buffer in;
    {
        char buf[4096];
        size_t n;

        while ((n = fread(buf, 1, sizeof(buf), is)) > 0)
            in.append(buf, n);
    }

    // Close file:

    fclose(is);

    // Deserialize and print instances to standard output:

    size_t pos = 0;

    while (in.size() != pos)
    {
        CIMNamespaceName ns;
        CIMInstance ci;

        if (DeserializeNameSpace(in, pos, ns) != 0 ||
            DeserializeInstance(in, pos, ci) != 0)
        {
            fprintf(stderr, "%s: malformed instance file: \"%s\"\n", 
                argv[0], argv[1]);
            exit(1);
        }

        printf("//\n");
        printf("// Namespace: %s\n", (const char*)ns.getString().getCString());
        printf("//\n");
        printf("\n");

        Buffer out;
        MofWriter::appendInstanceElement(out, ci);

        printf("%s\n", out.getData());
    }

    return 0;
}

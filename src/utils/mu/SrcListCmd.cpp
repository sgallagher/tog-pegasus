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
//%=============================================================================

#include <utils/mu/Dependency.h>
#include <utils/mu/SrcListCmd.h>
#include <cstdio>
#include <cstddef>

void PrintSrcDependency(const string& objectFileName, const string& fileName)
{
    printf("%s\n", fileName.c_str());
}

int SrcListCmdMain(int argc, char** argv)
{
    // Check arguments:

    if (argc == 1)
    {
        fprintf(stderr,
            "Usage: %s [-W]? [-Iinclude_dir]* [-Dsource_dir]? source_files...\n"
            "Where: \n"
            "    -W - warn about include files which cannot be found\n"
            "    -D - prepend this directory to source files\n"
            "    -I - search this directory for header files\n",
            argv[0]);
        exit(1);
    }

    // Extract the program name:

    static char *programName = argv[0];
    argc--;
    argv++;

    // Process all options:

    vector<string> includePath;
    string objectDir; // ignored
    string prependDir;
    bool warn = false;
    
    ProcessOptions(argc, argv, programName, includePath, objectDir, prependDir, 
        warn);

    // There must be at least one source file; print error if not:

    if (argc < 1)
    {
        ErrorExit(programName, "no source files given");
    }

    // Process each file:

    for (int i = 0; i < argc; i++)
    {
        string fileName;

        if (prependDir.size()){
            fileName = prependDir;
            fileName += '/';
            fileName += argv[i];
        }
        else
        {
            fileName = argv[i];
        }

        // Open the file:

        FILE* fp = fopen(argv[i], "rb");

        if (fp == NULL)
        {
            string message = "failed to open file: \"" + fileName + "\"";
            ErrorExit(programName, message);
        }

        const char* start = fileName.c_str();
        const char* dot = strrchr(start, '.');

        if( dot == NULL )
        {
            ErrorExit(programName, "bad extension: must be \".c\", \".cpp\"," 
                "or \".s\": " + fileName);
        }

        if ((strcmp(dot, ".cpp") != 0) &&
            (strcmp(dot, ".c") != 0) &&
            (strcmp(dot, ".s") != 0))
        {
            ErrorExit(programName, "bad extension: must be \".c\", \".cpp\"," 
                "or \".s\": " + fileName);
        }

        set<string, less<string> > cache;

        ProcessFile("", fileName, programName, fp, includePath, prependDir, 
            0, cache, PrintSrcDependency, warn);
    }

    return 0;
}

int SrcListCmd (const vector<string>& args)
{
    // Dummy up argc/argv structures and call DependCmdMain():

    int argc = static_cast<int>(args.size());
    char** argv = new char*[args.size()];

    for (int i = 0; i < argc; i++)
    {
        argv[i] = (char*)args[i].c_str();
    }

    int result = SrcListCmdMain(argc, argv);

    delete [] argv;

    return result;
}

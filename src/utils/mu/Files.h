//%=============================================================================
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
//------------------------------------------------------------------------------
//
// Author: Michael E. Brasher
//
//%=============================================================================

#ifndef _Files_h
#define _Files_h

#include "Config.h"
#include <string>
#include <vector>

bool GetCwd(string& path);

bool ChangeDir(const string& path);

bool RemoveFile(const string& path);

bool RemoveDir(const string& path);

bool RemoveFile(const string& path, bool recurse);

bool MakeDir(const string& path);

bool MkDirHier(const string& path);

bool GetDirEntries(const string& path, vector<string>& filenames);

bool MatchString(const string& pattern, const string& str);

void SplitPath(const string& path, string& dirname, string& basename);

bool Glob(const string& pattern, vector<string>& filenames_out);

bool TouchFile(const string& path);

bool CopyFile(const string& from_file, const string& to_file);

bool CopyFiles(const vector<string>& from, const string& to);

bool CompareFiles(
    const string& filename1, 
    const string& filename2, 
    size_t& offset);

bool GetFileSize(const string& path, size_t& size);

bool Exists(const string& path);

bool Writable(const string& path);

bool Readable(const string& path);

bool IsDir(const string& path);

#endif /* _Files_h */

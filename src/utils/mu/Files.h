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

#include <iostream>
#include <fstream>
#include <cstdio>
#include "Config.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <dirent.h>
#include <fcntl.h>
#include "Files.h"

bool GetCwd(string& path)
{
    char tmp[4096];

    getcwd(tmp, sizeof(tmp));
    path = tmp;
    return true;
}

bool ChangeDir(const string& path)
{
    return chdir(path.c_str()) == 0;
}

bool RemoveDir(const string& path)
{
    return rmdir(path.c_str()) == 0;
}

bool RemoveFile(const string& path)
{
    return unlink(path.c_str()) == 0;
}

bool MakeDir(const string& path)
{
    return mkdir(path.c_str(), 0777) == 0;
}

bool GetDirEntries(const string& path, vector<string>& filenames)
{
    filenames.clear();

    DIR* dir = opendir(path.c_str());

    if (!dir)
	return false;

    for (dirent* entry = readdir(dir); entry; entry = readdir(dir))
    {
	string name = entry->d_name;

	if (name != "." && name != "..")
	    filenames.push_back(name);
    }

    closedir(dir);

    return true;
}

bool TouchFile(const string& path)
{
    if (IsDir(path))
	return false;

    // Get file-size:

    struct stat sbuf;

    // If file does not exist:

    if (stat(path.c_str(), &sbuf) != 0)
    {
	int fd = open(path.c_str(), O_WRONLY | O_CREAT, 0666);

	if (fd < 0)
	    return false;

	close(fd);
	return true;
    }

    // File does exist:

    size_t size = sbuf.st_size;

    if (size == 0)
    {
	// Open file:

	int fd = open(path.c_str(), O_RDWR, 0666);

	if (fd < 0)
	    return false;

	char c = '\0';

	// Write one byte:

	if (write(fd, &c, sizeof(char)) != 1)
	    return false;

	// Truncate back to zero size:

	if (ftruncate(fd, size) < 0)
	    return false;

	// Close the file:

	close(fd);

	return true;
    }
    else
    {
	// Open the file:

	int fd = open(path.c_str(), O_RDWR, 0666);

	if (fd < 0)
	    return false;

	// Read first character, rewind, then rewrite it:

	char c;

	if (read(fd, &c, sizeof(char)) != 1)
	    return false;

	if (lseek(fd, 0, SEEK_SET) < 0)
	    return false;

	if (write(fd, &c, sizeof(char)) != 1)
	    return false;

	// Truncate file to force mod of times:

	if (ftruncate(fd, size) < 0)
	    return false;

	close(fd);
    }

    return true;
}

bool GetFileSize(const string& path, size_t& size)
{
    struct stat st;

    if (stat(path.c_str(), &st) != 0)
	return false;

    size = (size_t)(st.st_size);
    return true;
}

bool Exists(const string& path)
{
    return access(path.c_str(), F_OK) == 0;
}

bool Readable(const string& path)
{
    return access(path.c_str(), R_OK) == 0;
}

bool Writable(const string& path)
{
    return access(path.c_str(), W_OK) == 0;
}

bool IsDir(const string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

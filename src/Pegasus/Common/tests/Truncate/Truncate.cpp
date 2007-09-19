#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_PEGASUS;

int main(int argc, char** argv)
{
    const char FILENAME[] = "TestTruncate.dat";

    // Resolve the temporary directory.

    const char* rootdir;
    
    if (argc == 1)
    {
        const char* tmp = getenv ("PEGASUS_TMP");
        rootdir = tmp ? tmp : ".";
    }
    else if (argc == 2)
    {
        rootdir = argv[1];
    }
    else
    {
        fprintf(stderr, "Usage: %s directory\n", argv[0]);
        exit(1);
    }

    String path_ = String(rootdir) + String("/") + String(FILENAME);
    CString path(path_.getCString());

    // Create a file containing the alphabet.

    FILE* os = fopen(path, "wb");
    PEGASUS_TEST_ASSERT(os != NULL);

    size_t n = fwrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 26, os);
    PEGASUS_TEST_ASSERT(n == 26);

    fclose(os);

    // Truncate the file so that only "ABCDEFG" remains.

    bool flag = System::truncateFile(path, 7);
    PEGASUS_TEST_ASSERT(flag);

    // Read the file back to be sure it contains "ABCDEFG".

    FILE* is = fopen(path, "rb");
    PEGASUS_TEST_ASSERT(os != NULL);

    char buffer[4096];
    n = fread(buffer, 1, sizeof(buffer), os);
    PEGASUS_TEST_ASSERT(n == 7);
    PEGASUS_TEST_ASSERT(memcmp(buffer, "ABCDEFG", 7) == 0);

    fclose(is);

    // Remove the file:

    System::removeFile(path);

    printf("(%s) +++++ passed all tests\n", argv[0]);

    return 0;
}

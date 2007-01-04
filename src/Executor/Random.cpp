#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//==============================================================================
//
// FillRandomBytes1()
//
//     Fills *data* with *size* random bytes using the high-quality random
//     number generator device (/dev/urandom).
//
//==============================================================================

static int _fillRandomBytes1(unsigned char* data, size_t size)
{
    size_t numErrors = 0;
    const size_t MAX_ERRORS_TOLERATED = 20;

    int fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);

    if (fd < 0)
        return -1;

    while (size)
    {
        ssize_t n = read(fd, data, size);

        if (n < 1)
        {
            if (++numErrors == MAX_ERRORS_TOLERATED)
                break;

            if (errno == EAGAIN)
                continue;

            break;
        }

        size -= n;
        data += n;
    }

    close(fd);
    return size == 0 ? 0 : -1;
}

//==============================================================================
//
// FillRandomBytes2()
//
//     Fills *data* with *size* random bytes using the standard rand() function.
//
//==============================================================================

static void _fillRandomBytes2(unsigned char* data, size_t size)
{
    // Seed the random number generator.

    timeval tv;
    gettimeofday(&tv, 0);
    srand(tv.tv_usec);

    // Fill data with random bytes.

    while (size--)
        *data++ = rand();
}

//==============================================================================
//
// FillRandomBytes()
//
//==============================================================================

void FillRandomBytes(unsigned char* data, size_t size)
{
    if (_fillRandomBytes1(data, size) != 0)
        _fillRandomBytes2(data, size);
}

//==============================================================================
//
// RandBytesToHexASCII()
//
//     Converts the bytes given by *data* to a hexidecimal sequence of ASCII
//     characters. The *ascii* parameter must be twice size plus one (for the
//     null terminator).
//
//==============================================================================

void RandBytesToHexASCII(
    const unsigned char* data, 
    size_t size, 
    char* ascii)
{
    static char _hexDigits[] = 
    {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    size_t j = 0;

    for (size_t i = 0; i < size; i++)
    {
        unsigned char hi = data[i] >> 4;
        unsigned char lo = 0x0F & data[i];
        ascii[j++] = _hexDigits[hi];
        ascii[j++] = _hexDigits[lo];
    }

    ascii[j] = '\0';
}

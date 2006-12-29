
//==============================================================================
//
// Strlncpy()
//
//     This is a variation of the strlcpy() function as described by Todd C. 
//     Miller in his popular security paper entitled "strlcpy and strlcat - 
//     consistent, safe, string copy and concatenation".
//
//     Note that this implementation favors readability over efficiency. More
//     efficient implemetations are possible but would be to complicated
//     to verify in a security audit.
//
//==============================================================================

static size_t Strlncpy(char* dest, const char* src, size_t size, size_t n)
{
    size_t i;

    for (i = 0; i < n && src[i] && i + 1 < size; i++)
        dest[i] = src[i];

    if (size > 0)
        dest[i] = '\0';

    while (i < n && src[i])
        i++;

    return i;
}

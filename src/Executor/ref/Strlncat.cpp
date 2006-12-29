
//==============================================================================
//
// Strlncat()
//
//     This is a variation of the strlcat() function as described
//     by Todd C. Miller in his popular security paper entitled "strlcpy and 
//     strlcat - consistent, safe, string copy and concatenation".
//
//     Note that this implementation favors readability over efficiency. More
//     efficient implemetations are possible but would be to complicated
//     to verify in a security audit.
//
//==============================================================================

static size_t Strlncat(char* dest, const char* src, size_t size, size_t n)
{
    size_t i;
    size_t j;

    // Find dest null terminator.

    for (i = 0; i < size && dest[i]; i++)
        ;

    // If no-null terminator found, return size.

    if (i == size)
        return size;

    // Copy src characters to dest.

    for (j = 0; j < n && src[j] && i + 1 < size; i++, j++)
        dest[i] = src[j];

    // Null terminate size non-zero.

    if (size > 0)
        dest[i] = '\0';

    while (j < n && src[j])
    {
        j++;
        i++;
    }

    return i;
}

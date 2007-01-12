#include <Executor/Match.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    assert(Match("*", "xxx") == 0);
    assert(Match("abc*", "abc") == 0);
    assert(Match("abc*", "abcxyz") == 0);
    assert(Match("abc*xyz", "abc*xyz") == 0);
    assert(Match("abc*xyz", "abcxyz") == 0);
    assert(Match("abc*xyz", "abcXXXXXXXXXXXXXXXXXxxyz") == 0);
    assert(Match("", "") == 0);
    assert(Match("*", "") == 0);
    assert(Match("*xxx*", "xxx") == 0);

    printf("+++++ passed all tests\n");

    return 0;
}

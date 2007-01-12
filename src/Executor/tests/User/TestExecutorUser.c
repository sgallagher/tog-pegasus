#include <Executor/User.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main()
{
    char username[EXECUTOR_BUFFER_SIZE];
    int uid;
    int gid;

    assert(GetUserInfo("root", &uid, &gid) == 0);
    assert(uid == 0 && gid == 0);

    assert(GetUserName(uid, username) == 0);
    assert(strcmp(username, "root") == 0);

    printf("+++++ passed all tests\n");
    return 0;
}

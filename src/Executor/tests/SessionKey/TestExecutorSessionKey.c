#include <Executor/SessionKey.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

static char message[EXECUTOR_BUFFER_SIZE];
static int destructorCalls = 0;

void _destructor(long data)
{
    destructorCalls++;
    strcpy(message, (const char*)data);
}

int main()
{
    SessionKey key;

    /* NewSessionKey() */
    {
        key = NewSessionKey(999, (long)"999", _destructor, 0);
        assert(sizeof(key.data) == 33);
        assert(strlen(key.data) == 32);
    }

    /* GetSessionKeyAuthenticated() */
    {
        int auth = 1;
        assert(GetSessionKeyAuthenticated(&key, &auth) == 0);
        assert(auth == 0);
    }

    /* SetSessionKeyAuthenticated() */
    {
        int auth = 0;
        assert(SetSessionKeyAuthenticated(&key) == 0);
        assert(GetSessionKeyAuthenticated(&key, &auth) == 0);
        assert(auth == 1);
    }

    /* GetSessionKeyUid() */
    {
        int uid = 0;
        assert(GetSessionKeyUid(&key, &uid) == 0);
        assert(uid == 999);
    }

    /* GetSessionKeyData() */
    {
        long data;
        assert(GetSessionKeyData(&key, &data) == 0);
        assert(strcmp("999", (const char*)data) == 0);
    }

    /* DeleleteSessionKeyData() */
    {
        assert(DeleteSessionKeyData(&key) == 0);
        assert(destructorCalls == 1);
        assert(strcmp(message, "999") == 0);
    }

    /* DeleteSessionKey() */
    {
        assert(DeleteSessionKey(&key) == 0);
        assert(destructorCalls == 1);
    }

    printf("+++++ passed all tests\n");

    return 0;
}

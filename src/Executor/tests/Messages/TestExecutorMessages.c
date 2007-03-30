#include <Executor/Messages.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define CHECK(S1,S2) \
    do \
    { \
        assert(strcmp(MessageCodeToString(S1), S2) == 0); \
    } \
    while (0)

int main()
{
    CHECK((enum ExecutorMessageCode)0, "<unknown>");
    CHECK(EXECUTOR_PING_MESSAGE, "Ping");
    CHECK(EXECUTOR_OPEN_FILE_MESSAGE, "OpenFile");
    CHECK(EXECUTOR_START_PROVIDER_AGENT_MESSAGE, "StartProviderAgent");
    CHECK(EXECUTOR_DAEMONIZE_EXECUTOR_MESSAGE, "DaemonizeExecutor");
    CHECK(EXECUTOR_REMOVE_FILE_MESSAGE, "RemoveFile");
    CHECK(EXECUTOR_RENAME_FILE_MESSAGE, "RenameFile");
    CHECK(EXECUTOR_REAP_PROVIDER_AGENT, "ReapProviderAgent");
    CHECK(EXECUTOR_AUTHENTICATE_PASSWORD_MESSAGE, "AuthenticatePassword");
    CHECK(EXECUTOR_VALIDATE_USER_MESSAGE, "ValidateUser");
    CHECK(EXECUTOR_CHALLENGE_LOCAL_MESSAGE, "ChallengeLocal");
    CHECK(EXECUTOR_AUTHENTICATE_LOCAL_MESSAGE, "AuthenticateLocal");

    printf("+++++ passed all tests\n");

    return 0;
}

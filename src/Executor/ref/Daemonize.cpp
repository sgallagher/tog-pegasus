
//==============================================================================
//
// Daemonize()
//
//==============================================================================

void Daemonize()
{
    // Fork:

    int pid = (int)fork();

    if (pid < 0)
        Fatal(FL, "fork() failed");

    // Parent terminates, child continues.

    if (pid != 0)
    {
Log(LOG_INFO, "***1");
        signal(SIGUSR1, SigHandler);
Log(LOG_INFO, "***2");

        // Wait up to 10 seconds for great grandchild (cimservermain) to
        // send SIGUSR1, indicating that it is initialized.

Log(LOG_INFO, "***3");
        for (size_t i = 0; i < 10; i++)
        {
Log(LOG_INFO, "***4");
            if (TstBit(_signalMask, SIGUSR1))
            {
Log(LOG_INFO, "***5");
                break;
            }

Log(LOG_INFO, "***6");
            sleep(1);
        }

Log(LOG_INFO, "***7");
        _exit(0);
    }

    // Become session leader.

    if (setsid() < 0)
        Fatal(FL, "setsid() failed");

    // Ignore SIGHUP signal.

    signal(SIGHUP, SIG_IGN);

    // Fork again:

    pid = (int)fork();

    if (pid < 0)
        Fatal(FL, "fork() failed");

    // Parent terminates, child continues.

    if (pid != 0)
        _exit(0);

    // Change current working directory.

    chdir("/");

    // Close all file descriptors except for socket pair used to communicate
    // with cimservermain process.

    struct rlimit rlim;

    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
    {
        for (int i = 0; i < int(rlim.rlim_cur); i++)
            close(i);
    }

    // Redirect stdin, stdout, and stderr to /dev/null.

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    // We must re-open the log since we just closed it.

    OpenLog(true, "cimexecutor");
}

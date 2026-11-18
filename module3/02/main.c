#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "pubsub.h"

volatile sig_atomic_t stop = 0;

static void on_signal(int sig)
{
    (void)sig;
    stop = 1;
}

static int valid_topic(const char *t)
{
    return t[0] && strlen(t) < TOPIC_LEN && !strpbrk(t, ",\n");
}

int main(int argc, char *argv[])
{
    const char *mode = argc > 1 ? argv[1] : "";
    int ok = (!strcmp(mode, "-b") && argc == 2) ||
             (!strcmp(mode, "-p") && argc == 3) ||
             (!strcmp(mode, "-s") && argc >= 3);

    for (int i = 2; ok && i < argc; i++)
        ok = valid_topic(argv[i]);
    if (!ok)
    {
        fprintf(stderr, "Использование:\n  %s -b\n  %s -p тема\n  %s -s тема [тема ...]\n"
                        "(тема: до %d символов, без запятых)\n", argv[0], argv[0], argv[0], TOPIC_LEN - 1);
        return 2;
    }

    /* без SA_RESTART: msgrcv/msgsnd/sleep прерываются сигналом */
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    setvbuf(stdout, NULL, _IOLBF, 0);

    if (!strcmp(mode, "-b"))
        return run_broker();
    if (!strcmp(mode, "-p"))
        return run_publisher(argv[2]);
    return run_subscriber(argc - 2, argv + 2);
}
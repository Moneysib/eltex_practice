#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "pubsub.h"

static int open_queue(void)
{
    int qid = msgget(queue_key(), 0);

    if (qid < 0)
        fprintf(stderr, "Брокер не запущен (очереди нет)\n");
    return qid;
}

static int send_cmd(int qid, const char *cmd, const char *topic, int flags)
{
    struct msg m;

    m.mtype = 1;
    snprintf(m.text, TEXT_LEN, "%s,%d,%s", cmd, (int)getpid(), topic);
    return msgsnd(qid, &m, strlen(m.text) + 1, flags);
}

int run_publisher(const char *topic)
{
    struct msg m;
    int qid = open_queue();

    if (qid < 0)
        return 1;
    m.mtype = 1;
    for (int n = 1; !stop; n++)
    {
        snprintf(m.text, TEXT_LEN, "send,%d,%s\nсообщение %d", (int)getpid(), topic, n);
        if (msgsnd(qid, &m, strlen(m.text) + 1, 0) < 0)
        {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "Издатель %d: очередь недоступна\n", (int)getpid());
            return 0;
        }
        printf("Издатель %d: отправлено сообщение %d по теме '%s'\n", (int)getpid(), n, topic);
        sleep(5);
    }
    printf("Издатель %d: завершение по сигналу\n", (int)getpid());
    return 0;
}

int run_subscriber(int n, char *topics[])
{
    struct msg m;
    int qid = open_queue();

    if (qid < 0)
        return 1;
    for (int i = 0; i < n; i++)
        if (send_cmd(qid, "subscribe", topics[i], 0) < 0)
        {
            perror("subscribe");
            return 1;
        }
    printf("Подписчик %d ждет сообщения\n", (int)getpid());

    while (!stop)
    {
        if (msgrcv(qid, &m, TEXT_LEN, getpid(), 0) < 0)
        {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "Подписчик %d: очередь недоступна\n", (int)getpid());
            return 0;
        }
        m.text[TEXT_LEN - 1] = '\0';
        printf("Подписчик %d получил: %s\n", (int)getpid(), m.text);
    }

    for (int i = 0; i < n; i++)
        send_cmd(qid, "unsubscribe", topics[i], IPC_NOWAIT);
    printf("Подписчик %d: завершение по сигналу\n", (int)getpid());
    return 0;
}
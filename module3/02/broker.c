#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "pubsub.h"

#define WAIT_SEC 3

struct entry
{
    pid_t pid;
    char  topic[TOPIC_LEN];
};

static struct entry *tab;
static int cnt;

static int find(pid_t pid, const char *topic)
{
    for (int i = 0; i < cnt; i++)
        if (tab[i].pid == pid && strcmp(tab[i].topic, topic) == 0)
            return i;
    return -1;
}

static void add(pid_t pid, const char *topic)
{
    if (find(pid, topic) >= 0)
        return;
    struct entry *p = realloc(tab, (cnt + 1) * sizeof *tab);
    if (!p)
        return;
    tab = p;
    tab[cnt].pid = pid;
    snprintf(tab[cnt].topic, TOPIC_LEN, "%s", topic);
    cnt++;
}

static void del(pid_t pid, const char *topic)
{
    int i = find(pid, topic);
    if (i >= 0)
        tab[i] = tab[--cnt];
}

static void forward(int qid, pid_t from, const char *topic, const char *payload)
{
    struct msg out;

    for (int i = 0; i < cnt; i++)
    {
        if (tab[i].topic[0] == '\0' || strcmp(tab[i].topic, topic) != 0)
            continue;
        out.mtype = tab[i].pid;
        snprintf(out.text, TEXT_LEN, "[%s] от %d: %s", topic, (int)from, payload);
        if (msgsnd(qid, &out, strlen(out.text) + 1, IPC_NOWAIT) < 0)
            fprintf(stderr, "Брокер: не доставлено подписчику %d: %s\n", (int)tab[i].pid, strerror(errno));
    }
}

static void handle(int qid, const char *text)
{
    char cmd[16], topic[TOPIC_LEN];
    int pid, n = 0;

    if (sscanf(text, "%15[^,],%d,%31[^\n]%n", cmd, &pid, topic, &n) != 3)
    {
        fprintf(stderr, "Брокер: неверное сообщение '%s'\n", text);
        return;
    }
    if (strcmp(cmd, "subscribe") == 0)
    {
        add(pid, topic);
        printf("Брокер: подписка %d на '%s'\n", pid, topic);
    }
    else if (strcmp(cmd, "unsubscribe") == 0)
    {
        del(pid, topic);
        printf("Брокер: отписка %d от '%s'\n", pid, topic);
    }
    else if (strcmp(cmd, "send") == 0)
    {
        add(pid, "");
        printf("Брокер: сообщение от %d по теме '%s'\n", pid, topic);
        forward(qid, pid, topic, text[n] == '\n' ? text + n + 1 : text + n);
    }
}

int run_broker(void)
{
    struct msg m;
    struct msqid_ds ds;

    int qid = msgget(queue_key(), IPC_CREAT | IPC_EXCL | 0666);
    if (qid < 0)
    {
        if (errno == EEXIST)
            fprintf(stderr, "Брокер уже запущен (очередь существует)\n");
        else
            perror("msgget");
        return 1;
    }
    printf("Брокер запущен, очередь %d\n", qid);

    while (!stop)
    {
        if (msgrcv(qid, &m, TEXT_LEN, 1, 0) < 0)
        {
            if (errno == EINTR)
                continue;
            perror("msgrcv");
            break;
        }
        m.text[TEXT_LEN - 1] = '\0';
        handle(qid, m.text);
    }

    printf("Брокер завершает работу\n");
    for (int i = 0; i < cnt; i++)
    {
        int seen = 0;
        for (int j = 0; j < i; j++)
            seen |= tab[j].pid == tab[i].pid;
        if (!seen)
            kill(tab[i].pid, SIGINT);
    }

    for (int t = 0; t < WAIT_SEC; t++)
    {
        while (msgrcv(qid, &m, TEXT_LEN, 1, IPC_NOWAIT) >= 0)
            ;
        if (msgctl(qid, IPC_STAT, &ds) == 0 && ds.msg_qnum == 0)
            break;
        sleep(1);
    }
    msgctl(qid, IPC_RMID, NULL);
    free(tab);
    return 0;
}
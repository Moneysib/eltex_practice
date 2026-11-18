#ifndef PUBSUB_H_
#define PUBSUB_H_

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define TEXT_LEN  256
#define TOPIC_LEN 32

struct msg
{
    long mtype;
    char text[TEXT_LEN];
};

extern volatile sig_atomic_t stop;

static inline key_t queue_key(void)
{
    return ftok("/tmp", 'P');
}

int run_broker(void);
int run_publisher(const char *topic);
int run_subscriber(int n, char *topics[]);

#endif
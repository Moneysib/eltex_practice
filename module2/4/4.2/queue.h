#ifndef QUEUE_H
#define QUEUE_H

#define MSG_LEN 64
#define PRIO_MIN 0
#define PRIO_MAX 255

typedef struct Message
{
    int priority;
    char text[MSG_LEN];
} Message;

typedef struct Item
{
    Message msg;
    struct Item *next;
} Item;

Item *push(Item *head, int priority, const char *text);

int pop_first(Item **head, Message *out);
int pop_priority(Item **head, int priority, Message *out);
int pop_min_priority(Item **head, int min, Message *out);

void print_queue(const Item *head);
Item *delete_queue(Item *head);

#endif
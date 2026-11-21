#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

Item *push(Item *head, int priority, const char *text)
{
    if (priority < PRIO_MIN || priority > PRIO_MAX)
    {
        fprintf(stderr, "Bad priority %d (must be %d..%d)\n", priority, PRIO_MIN, PRIO_MAX);
        return head;
    }
    Item *item = malloc(sizeof(Item));
    if (item == NULL)
        return head;
    item->msg.priority = priority;
    strncpy(item->msg.text, text, MSG_LEN - 1);
    item->msg.text[MSG_LEN - 1] = '\0';

    if (head == NULL || priority > head->msg.priority)
    {
        item->next = head;
        return item;
    }
    Item *tmp = head;
    while (tmp->next != NULL && tmp->next->msg.priority >= priority)
        tmp = tmp->next;
    item->next = tmp->next;
    tmp->next = item;
    return head;
}

static int take(Item **link, Message *out)
{
    Item *item = *link;
    *out = item->msg;
    *link = item->next;
    free(item);
    return 1;
}

int pop_first(Item **head, Message *out)
{
    if (*head == NULL)
        return 0;
    return take(head, out);
}

int pop_priority(Item **head, int priority, Message *out)
{
    for (Item **link = head; *link != NULL; link = &(*link)->next)
        if ((*link)->msg.priority == priority)
            return take(link, out);
    return 0;
}

int pop_min_priority(Item **head, int min, Message *out)
{
    if (*head == NULL || (*head)->msg.priority < min)
        return 0;
    return take(head, out);
}

void print_queue(const Item *head)
{
    if (head == NULL)
    {
        printf("Queue empty!\n");
        return;
    }
    for (const Item *item = head; item != NULL; item = item->next)
        printf("  [%3d] %s\n", item->msg.priority, item->msg.text);
}

Item *delete_queue(Item *head)
{
    while (head != NULL)
    {
        Item *item = head;
        head = head->next;
        free(item);
    }
    return NULL;
}
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contacts.h"

typedef struct Node
{
    Contact data;
    struct Node *next;
    struct Node *prev;
} Node;

_Static_assert(offsetof(Node, data) == 0, "data должно быть первым полем узла");

static Node *head = NULL;
static int count = 0;

static int compare(const Contact *a, const Contact *b)
{
    int r = strcmp(a->last, b->last);
    return r != 0 ? r : strcmp(a->first, b->first);
}

static void insert_before(Node *pos, Node *n)
{
    n->next = pos;
    n->prev = pos->prev;
    pos->prev->next = n;
    pos->prev = n;
}

static void link_sorted(Node *n)
{
    if (head == NULL)
    {
        n->next = n;
        n->prev = n;
        head = n;
        return;
    }

    Node *tmp = head;
    do
    {
        if (compare(&n->data, &tmp->data) < 0)
        {
            insert_before(tmp, n);
            if (tmp == head)
                head = n;
            return;
        }
        tmp = tmp->next;
    } while (tmp != head);

    insert_before(head, n);
}

static void unlink_node(Node *n)
{
    if (n->next == n)
    {
        head = NULL;
        return;
    }
    n->prev->next = n->next;
    n->next->prev = n->prev;
    if (n == head)
        head = n->next;
}

static Node *find_node(int index)
{
    if (index < 0 || index >= count)
        return NULL;
    Node *tmp = head;
    while (index-- > 0)
        tmp = tmp->next;
    return tmp;
}

int book_add(const Contact *c)
{
    if (!c->last[0] || !c->first[0])
        return -2;

    Node *n = malloc(sizeof(Node));
    if (!n)
        return -1;
    n->data = *c;
    link_sorted(n);
    count++;
    return 0;
}

int book_remove(int index)
{
    Node *n = find_node(index);
    if (!n)
        return -1;
    unlink_node(n);
    free(n);
    count--;
    return 0;
}

Contact *book_get(int index)
{
    Node *n = find_node(index);
    return n ? &n->data : NULL;
}

void book_reorder(Contact *c)
{
    Node *n = (Node *)c;
    unlink_node(n);
    link_sorted(n);
}

static void print_list(const char *name, const char list[][LEN])
{
    for (int i = 0; i < N; i++)
        if (list[i][0])
            printf("   %s: %s\n", name, list[i]);
}

void book_print(void)
{
    if (head == NULL)
    {
        printf("Список пуст\n");
        return;
    }

    int i = 1;
    Node *tmp = head;
    do
    {
        const Contact *c = &tmp->data;
        printf("%d. %s %s\n", i++, c->last, c->first);
        if (c->work[0])
            printf("   Работа: %s\n", c->work);
        if (c->post[0])
            printf("   Должность: %s\n", c->post);
        print_list("Телефон", c->phone);
        print_list("Email", c->email);
        print_list("Соцсеть", c->social);
        print_list("Мессенджер", c->messenger);
        tmp = tmp->next;
    } while (tmp != head);
}

void book_clear(void)
{
    if (head == NULL)
        return;

    Node *tmp = head;
    do
    {
        Node *del = tmp;
        tmp = tmp->next;
        free(del);
    } while (tmp != head);

    head = NULL;
    count = 0;
}
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

void book_seed(void)
{
    static const struct {
        const char *last;
        const char *first;
        const char *work;
        const char *post;
        const char *phone;
    } items[] = {
        {"Смирнов", "Алексей", "Eltex", "Инженер", "+79000000001"},
        {"Иванов", "Борис", "Eltex", "Тестировщик", "+79000000002"},
        {"Иванов", "Антон", "КБ", "Разработчик", "+79000000003"},
        {"Петров", "Сергей", "Связь", "Администратор", "+79000000004"},
        {"Соколов", "Дмитрий", "Eltex", "Тимлид", "+79000000005"},
        {"Волков", "Павел", "Лаб", "Аналитик", "+79000000006"},
        {"Зайцев", "Никита", "Eltex", "Инженер", "+79000000007"},
        {"Морозов", "Илья", "Связь", "Инженер", "+79000000008"},
        {"Лебедев", "Артём", "КБ", "Конструктор", "+79000000009"},
        {"Козлов", "Максим", "Eltex", "DevOps", "+79000000010"},
        {"Новиков", "Егор", "Лаб", "Инженер", "+79000000011"},
        {"Фёдоров", "Роман", "Связь", "Руководитель", "+79000000012"},
        {"Орлов", "Кирилл", "Eltex", "Разработчик", "+79000000013"},
        {"Андреев", "Денис", "КБ", "Инженер", "+79000000014"},
        {"Макаров", "Олег", "Лаб", "Тестировщик", "+79000000015"},
        {"Никитин", "Глеб", "Eltex", "Аналитик", "+79000000016"},
        {"Захаров", "Вадим", "Связь", "Инженер", "+79000000017"},
        {"Белов", "Тимур", "КБ", "Разработчик", "+79000000018"},
        {"Громов", " Виктор", "Eltex", "Инженер", "+79000000019"},
        {"Попов", "Андрей", "Лаб", "Архитектор", "+79000000020"},
    };

    size_t n = sizeof(items) / sizeof(items[0]);
    for (size_t i = 0; i < n; i++)
    {
        Contact c;
        memset(&c, 0, sizeof c);
        snprintf(c.last, LEN, "%s", items[i].last);
        snprintf(c.first, LEN, "%s", items[i].first);
        snprintf(c.work, LEN, "%s", items[i].work);
        snprintf(c.post, LEN, "%s", items[i].post);
        snprintf(c.phone[0], LEN, "%s", items[i].phone);
        book_add(&c);
    }
}
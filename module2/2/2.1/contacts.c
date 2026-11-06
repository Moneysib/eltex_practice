#include <stdio.h>
#include <string.h>
#include "contacts.h"

static Contact book[MAX_CONTACTS];
static int count = 0;

int book_add(const Contact *c)
{
    if (!c->last[0] || !c->first[0])
        return -2;
    if (count >= MAX_CONTACTS)
        return -1;
    book[count++] = *c;
    return 0;
}

int book_remove(int index)
{
    if (index < 0 || index >= count)
        return -1;
    memmove(&book[index], &book[index + 1], (count - index - 1) * sizeof(Contact));
    count--;
    return 0;
}

Contact *book_get(int index)
{
    if (index < 0 || index >= count)
        return NULL;
    return &book[index];
}

static void print_list(const char *name, const char list[][LEN])
{
    for (int i = 0; i < N; i++)
        if (list[i][0])
            printf("   %s: %s\n", name, list[i]);
}

void book_print()
{
    if (count == 0) printf("Список пуст\n");
    for (int i = 0; i < count; i++)
    {
        const Contact *c = &book[i];
        printf("%d. %s %s\n", i + 1, c->last, c->first);
        if (c->work[0])
            printf("   Работа: %s\n", c->work);
        if (c->post[0])
            printf("   Должность: %s\n", c->post);
        print_list("Телефон", c->phone);
        print_list("Email", c->email);
        print_list("Соцсеть", c->social);
        print_list("Мессенджер", c->messenger);
    }
}
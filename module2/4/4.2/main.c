#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "queue.h"

static int read_int(const char *prompt, int *value)
{
    char buf[32];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;
    return sscanf(buf, "%d", value) == 1;
}

static void show(const char *what, int found, const Message *m)
{
    if (found)
        printf("%s -> [%3d] %s\n", what, m->priority, m->text);
    else
        printf("%s -> not found\n", what);
}

static Item *generate(Item *head, int count)
{
    char text[MSG_LEN];
    for (int i = 1; i <= count; i++)
    {
        int prio = (rand() % 4) * 85;
        snprintf(text, MSG_LEN, "generated #%d", i);
        head = push(head, prio, text);
        printf("generated: [%3d] %s\n", prio, text);
    }
    return head;
}

static void print_menu()
{
    printf("\n1 - добавить элемент\n"
           "2 - извлечь первый элемент\n"
           "3 - извлечь элемент с указанным приоритетом\n"
           "4 - извлечь элемент с приоритетом не меньше чем заданный\n"
           "5 - вывести очередь\n"
           "6 - сгенерировать элементы\n"
           "0 - выход\n");
}

int main()
{
    Item *head = NULL;
    Message m;
    int cmd, prio;
    char text[MSG_LEN];

    srand(time(NULL));

    while (1)
    {
        print_menu();
        if (!read_int("> ", &cmd) || cmd == 0)
            break;

        switch (cmd)
        {
        case 1:
            if (!read_int("priority (0-255): ", &prio))
                break;
            printf("text: ");
            if (fgets(text, MSG_LEN, stdin) == NULL)
                break;
            text[strcspn(text, "\n")] = '\0';
            head = push(head, prio, text);
            break;
        case 2:
            show("pop_first", pop_first(&head, &m), &m);
            break;
        case 3:
            if (!read_int("priority: ", &prio))
                break;
            show("pop_priority", pop_priority(&head, prio, &m), &m);
            break;
        case 4:
            if (!read_int("min priority: ", &prio))
                break;
            show("pop_min_priority", pop_min_priority(&head, prio, &m), &m);
            break;
        case 5:
            print_queue(head);
            break;
        case 6:
            head = generate(head, 10);
            break;
        default:
            printf("Unknown command\n");
        }
    }

    head = delete_queue(head);
    return 0;
}
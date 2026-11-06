#include <stdio.h>
#include <string.h>
#include "contacts.h"

static void input(const char *prompt, char *dst)
{
    printf("%s", prompt);
    if (!fgets(dst, LEN, stdin))
        dst[0] = '\0';
    size_t n = strcspn(dst, "\n");
    if (dst[n] == '\0')
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
    dst[n] = '\0';
}

static int input_int(const char *prompt)
{
    char s[LEN];
    int v;
    input(prompt, s);
    return sscanf(s, "%d", &v) == 1 ? v : -1;
}

static void input_list(const char *name, char list[][LEN])
{
    for (int i = 0; i < N; i++)
    {
        char prompt[LEN];
        snprintf(prompt, sizeof prompt, "%s %d (Enter - пропустить): ", name, i + 1);
        input(prompt, list[i]);
        if (list[i][0] == '\0') break;
    }
}

static void fill(Contact *c)
{
    memset(c, 0, sizeof *c);
    input("Фамилия (обязательно): ", c->last);
    input("Имя (обязательно): ", c->first);
    input("Место работы: ", c->work);
    input("Должность: ", c->post);
    input_list("Телефон", c->phone);
    input_list("Email", c->email);
    input_list("Соцсеть", c->social);
    input_list("Мессенджер", c->messenger);
}

static void report(int rc, const char *okMessage)
{
    if (rc == 0)
        printf("%s\n", okMessage);
    else if (rc == -2)
        printf("Ошибка: фамилия и имя обязательны\n");
    else
        printf("Ошибка: книга переполнена или неверный номер\n");
}

static void edit_field(const char *title, char *field, int required)
{
    char buf[LEN];
    printf("%s: %s\n", title, field[0] ? field : "(пусто)");
    input("Новое значение (Enter - оставить, \"-\" - очистить): ", buf);

    if (buf[0] == '\0') return;
    if (strcmp(buf, "-") == 0)
    {
        if (required)
            printf("Это поле обязательно, очистить нельзя\n");
        else
            field[0] = '\0';
    }
    else
        strcpy(field, buf);
}

static void compact(char list[][LEN])
{
    int j = 0;
    for (int i = 0; i < N; i++)
    {
        if (list[i][0] == '\0')
            continue;
        if (i != j)
        {
            strcpy(list[j], list[i]);
            list[i][0] = '\0';
        }
        j++;
    }
}

static void edit_list(const char *title, char list[][LEN])
{
    printf("%s (максимум %d):\n", title, N);
    for (int i = 0; i < N; i++)
    {
        char prompt[LEN + 32], buf[LEN];
        snprintf(prompt, sizeof prompt, "  %d [%s]: ", i + 1, list[i][0] ? list[i] : "пусто");
        input(prompt, buf);

        if (buf[0] == '\0')
        {
            if (list[i][0] == '\0') break;
            continue;
        }
        if (strcmp(buf, "-") == 0)
            list[i][0] = '\0';
        else
            strcpy(list[i], buf);
    }
    compact(list);
}

static void action_edit(void)
{
    book_print();
    Contact *c = book_get(input_int("Номер контакта: ") - 1);
    if (!c)
    {
        printf("Нет такого контакта\n");
        return;
    }

    while (1)
    {
        printf("\nРедактирование: %s %s\n", c->last, c->first);
        printf("1 - Фамилия\n2 - Имя\n3 - Место работы\n4 - Должность\n"
               "5 - Телефоны\n6 - Email\n7 - Соцсети\n8 - Мессенджеры\n0 - Выход\n");

        int item = input_int("> ");
        if (feof(stdin))
            return;
        switch (item)
        {
            case 1:
                edit_field("Фамилия", c->last, 1);
                break;
            case 2:
                edit_field("Имя", c->first, 1);
                break;
            case 3:
                edit_field("Место работы", c->work, 0);
                break;
            case 4:
                edit_field("Должность", c->post, 0);
                break;
            case 5:
                edit_list("Телефоны", c->phone);
                break;
            case 6:
                edit_list("Email", c->email);
                break;
            case 7:
                edit_list("Соцсети", c->social);
                break;
            case 8:
                edit_list("Мессенджеры", c->messenger);
                break;
            case 0:
                return;
            default: printf("Неизвестный пункт\n");
        }
    }
}

int main(void)
{
    while (1) {
        printf("\n1 - добавить\n2 - показать\n3 - изменить\n4 - удалить\n0 - выход\n");
        int choice = input_int("> ");
        if (choice == 0 || feof(stdin))
            break;
        if (choice == 1)
        {
            Contact c;
            fill(&c);
            report(book_add(&c), "Контакт добавлен");
        }
        else if (choice == 2)
            book_print();
        else if (choice == 3)
            action_edit();
        else if (choice == 4)
        {
            book_print();
            report(book_remove(input_int("Номер контакта: ") - 1), "Контакт удалён");
        }
        else
            printf("Неизвестная команда\n");
    }
    return 0;
}
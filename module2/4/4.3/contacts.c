#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contacts.h"

#define REBALANCE_EVERY 5

typedef struct Node {
    Contact data;
    struct Node *left;
    struct Node *right;
} Node;

_Static_assert(offsetof(Node, data) == 0, "data должно быть первым полем узла");

static Node *root = NULL;
static int count = 0;
static int ops_since_balance = 0;

static int compare(const Contact *a, const Contact *b)
{
    int r = strcmp(a->last, b->last);
    return r != 0 ? r : strcmp(a->first, b->first);
}

static Node *insert_node(Node *cur, Node *n)
{
    if (!cur)
        return n;
    if (compare(&n->data, &cur->data) < 0)
        cur->left = insert_node(cur->left, n);
    else
        cur->right = insert_node(cur->right, n);
    return cur;
}

static Node *detach_min(Node *cur, Node **min_out)
{
    if (!cur->left)
    {
        *min_out = cur;
        return cur->right;
    }
    cur->left = detach_min(cur->left, min_out);
    return cur;
}

static Node *splice_out(Node *cur)
{
    if (!cur->left)
        return cur->right;
    if (!cur->right)
        return cur->left;

    Node *m = NULL;
    Node *new_right = detach_min(cur->right, &m);
    m->left = cur->left;
    m->right = new_right;
    return m;
}

static Node *delete_node_ptr(Node *cur, const Node *target, int *found)
{
    if (!cur)
        return NULL;

    if (cur == target)
    {
        *found = 1;
        return splice_out(cur);
    }

    int left_found = 0;
    cur->left = delete_node_ptr(cur->left, target, &left_found);
    if (left_found)
    {
        *found = 1;
        return cur;
    }

    int right_found = 0;
    cur->right = delete_node_ptr(cur->right, target, &right_found);
    if (right_found)
        *found = 1;

    return cur;
}

static Node *find_by_index(Node *cur, int *idx)
{
    if (!cur)
        return NULL;
    Node *res = find_by_index(cur->left, idx);
    if (res)
        return res;
    if (*idx == 0)
        return cur;
    (*idx)--;
    return find_by_index(cur->right, idx);
}

static void collect_nodes(Node *cur, Node **arr, int *pos)
{
    if (!cur)
        return;
    collect_nodes(cur->left, arr, pos);
    arr[(*pos)++] = cur;
    collect_nodes(cur->right, arr, pos);
}

static Node *build_balanced(Node **arr, int l, int r)
{
    if (l > r)
        return NULL;
    int mid = l + (r - l) / 2;
    Node *n = arr[mid];
    n->left = build_balanced(arr, l, mid - 1);
    n->right = build_balanced(arr, mid + 1, r);
    return n;
}

#ifdef DEBUG
static void print_tree_debug(const Node *cur, int depth)
{
    if (!cur)
        return;
    print_tree_debug(cur->right, depth + 1);
    for (int i = 0; i < depth; i++)
        printf("    ");
    printf("-> %s %s\n", cur->data.last, cur->data.first);
    print_tree_debug(cur->left, depth + 1);
}
#endif

static void maybe_rebalance(void)
{
    if (++ops_since_balance < REBALANCE_EVERY || count <= 1)
        return;
    ops_since_balance = 0;

    Node **arr = malloc((size_t)count * sizeof(Node *));
    if (!arr)
        return;

    #ifdef DEBUG
        printf("\n[DEBUG] Дерево ДО балансировки:\n");
        print_tree_debug(root, 0);
    #endif
        int pos = 0;
        collect_nodes(root, arr, &pos);
        root = build_balanced(arr, 0, pos - 1);
        free(arr);
    #ifdef DEBUG
        printf("[DEBUG] Дерево ПОСЛЕ балансировки:\n");
        print_tree_debug(root, 0);
    #endif
}

int book_add(const Contact *c)
{
    if (!c->last[0] || !c->first[0])
        return -2;

    Node *n = malloc(sizeof(Node));
    if (!n)
        return -1;
    n->data = *c;
    n->left = NULL;
    n->right = NULL;

    root = insert_node(root, n);
    count++;
    maybe_rebalance();
    return 0;
}

int book_remove(int index)
{
    if (index < 0 || index >= count)
        return -1;

    int idx = index;
    Node *target = find_by_index(root, &idx);
    if (!target)
        return -1;

    int found = 0;
    root = delete_node_ptr(root, target, &found);
    if (!found)
        return -1;

    free(target);
    count--;
    maybe_rebalance();
    return 0;
}

Contact *book_get(int index)
{
    if (index < 0 || index >= count)
        return NULL;
    int idx = index;
    Node *n = find_by_index(root, &idx);
    return n ? &n->data : NULL;
}

void book_reorder(Contact *c)
{
    Node *n = (Node *)c;
    int found = 0;
    root = delete_node_ptr(root, n, &found);
    n->left = NULL;
    n->right = NULL;
    root = insert_node(root, n);
    maybe_rebalance();
}

static void print_list(const char *name, const char list[][LEN])
{
    for (int i = 0; i < N; i++)
        if (list[i][0])
            printf("   %s: %s\n", name, list[i]);
}

static void print_inorder(const Node *cur, int *num)
{
    if (!cur)
        return;
    print_inorder(cur->left, num);

    const Contact *c = &cur->data;
    printf("%d. %s %s\n", (*num)++, c->last, c->first);
    if (c->work[0])
        printf("   Работа: %s\n", c->work);
    if (c->post[0])
        printf("   Должность: %s\n", c->post);
    print_list("Телефон", c->phone);
    print_list("Email", c->email);
    print_list("Соцсеть", c->social);
    print_list("Мессенджер", c->messenger);

    print_inorder(cur->right, num);
}

void book_print(void)
{
    if (!root)
    {
        printf("Список пуст\n");
        return;
    }
    int num = 1;
    print_inorder(root, &num);
}

static void free_tree(Node *cur)
{
    if (!cur)
        return;
    free_tree(cur->left);
    free_tree(cur->right);
    free(cur);
}

void book_clear(void)
{
    free_tree(root);
    root = NULL;
    count = 0;
    ops_since_balance = 0;
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
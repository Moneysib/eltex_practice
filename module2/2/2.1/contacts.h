#ifndef CONTACTS_H_
#define CONTACTS_H_

#define MAX_CONTACTS 100
#define LEN 64
#define N 3

typedef struct {
    char last[LEN], first[LEN], work[LEN], post[LEN];
    char phone[N][LEN], email[N][LEN], social[N][LEN], messenger[N][LEN];
} Contact;

int  book_add(const Contact *c);
int  book_remove(int index);

Contact *book_get(int index);
void book_print(void);

#endif
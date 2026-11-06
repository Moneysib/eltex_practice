#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "ipnet.h"

#define SHOW_LIMIT 20

static uint32_t random_ip(void)
{
    return ((uint32_t)(rand() & 0xFFFF) << 16) | (uint32_t)(rand() & 0xFFFF);
}

int main(int argc, char *argv[])
{
    uint32_t gateway, mask;

    if (argc != 4 || parse_ip(argv[1], &gateway) != 0 || parse_ip(argv[2], &mask) != 0 || atol(argv[3]) <= 0)
    {
        fprintf(stderr, "Использование: %s <IP шлюза> <маска> <число пакетов>\nПример: %s 192.168.1.1 255.255.255.0 1000\n", argv[0], argv[0]);
        return 1;
    }
    long n = atol(argv[3]);

    srand((unsigned)time(NULL));

    long own = 0;
    for (long i = 0; i < n; i++)
    {
        uint32_t dst = random_ip();
        int inside = same_subnet(gateway, dst, mask);
        if (inside)
            own++;

        if (n <= SHOW_LIMIT)
        {
            char text[16];
            ip_to_string(dst, text);
            printf("%-15s -> %s\n", text, inside ? "своя подсеть" : "другая сеть");
        }
    }

    printf("\nВсего пакетов:   %ld\n", n);
    printf("В своей подсети: %ld (%.2f%%)\n", own, 100.0 * own / n);
    printf("В других сетях:  %ld (%.2f%%)\n", n - own, 100.0 * (n - own) / n);
    return 0;
}
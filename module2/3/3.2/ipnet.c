#include <stdio.h>
#include "ipnet.h"

int parse_ip(const char *s, uint32_t *ip)
{
    unsigned o[4];
    int used = 0;

    if (sscanf(s, "%u.%u.%u.%u%n", &o[0], &o[1], &o[2], &o[3], &used) != 4 || s[used] != '\0')
        return -1;
    for (int i = 0; i < 4; i++)
        if (o[i] > 255)
            return -1;

    *ip = ((uint32_t)o[0] << 24) | ((uint32_t)o[1] << 16) | ((uint32_t)o[2] << 8)  |  (uint32_t)o[3];
    return 0;
}

void ip_to_string(uint32_t ip, char out[16])
{
    snprintf(out, 16, "%u.%u.%u.%u", (unsigned)((ip >> 24) & 0xFF), (unsigned)((ip >> 16) & 0xFF), (unsigned)((ip >> 8) & 0xFF),  (unsigned)(ip & 0xFF));
}

int same_subnet(uint32_t a, uint32_t b, uint32_t mask)
{
    return ((a & mask) ^ (b & mask)) == 0;
}

int is_valid_mask(uint32_t mask)
{
    uint32_t inv = ~mask;
    return (inv & (inv + 1)) == 0;
}
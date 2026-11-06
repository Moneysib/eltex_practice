#ifndef IPNET_H_
#define IPNET_H_

#include <stdint.h>

int  parse_ip(const char *s, uint32_t *ip);
void ip_to_string(uint32_t ip, char out[16]);
int  same_subnet(uint32_t a, uint32_t b, uint32_t mask);

#endif
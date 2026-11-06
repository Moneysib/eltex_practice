#ifndef PERMS_H_
#define PERMS_H_

int  parse_numeric(const char *s, unsigned *mode);
int  parse_letters(const char *s, unsigned *mode);
int  apply_chmod(unsigned *mode, const char *cmd);
int  mode_from_file(const char *path, unsigned *mode);
void print_mode(unsigned mode);

#endif
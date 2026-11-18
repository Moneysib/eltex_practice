#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "transfer.h"
 
struct header
{
    char  name[NAME_LEN];
    off_t size;
};
 
static const char READY = 'R';
 
static int wait_ready(int rfd)
{
    char c;
 
    return read(rfd, &c, 1) == 1 && c == READY ? 0 : -1;
}
 
int send_file(int wfd, int rfd, const char *path)
{
    struct stat st;
    struct header h;
    char buf[BLOCK_SIZE];
    int rc = 0;
 
    int in = open(path, O_RDONLY);
    if (in < 0)
    {
        fprintf(stderr, "Ошибка: не удалось открыть '%s': %s\n", path, strerror(errno));
        return 1;
    }
    if (fstat(in, &st) < 0 || !S_ISREG(st.st_mode) || strlen(path) >= NAME_LEN)
    {
        fprintf(stderr, "Ошибка: '%s' не обычный файл или слишком длинное имя\n", path);
        close(in);
        return 1;
    }
 
    memset(&h, 0, sizeof h);
    strcpy(h.name, path);
    h.size = st.st_size;
 
    if (wait_ready(rfd) != 0 || write(wfd, &h, sizeof h) != sizeof h)
        rc = -1;
 
    off_t left = h.size;
    while (rc == 0 && left > 0)
    {
        ssize_t r = read(in, buf, left < BLOCK_SIZE ? left : BLOCK_SIZE);
        if (r <= 0 || write(wfd, buf, r) != r)
            rc = -1;
        else
            left -= r;
    }
    if (rc < 0)
        fprintf(stderr, "Ошибка: передача '%s' прервана\n", path);
    close(in);
    return rc;
}
 
int send_done(int wfd, int rfd)
{
    struct header h;
 
    memset(&h, 0, sizeof h);
    return wait_ready(rfd) == 0 && write(wfd, &h, sizeof h) == sizeof h ? 0 : -1;
}
 
int receive_files(int rfd, int wfd)
{
    struct header h;
    char buf[BLOCK_SIZE], outname[NAME_LEN + 8];
    int status = 0;
    ssize_t n;
 
    write(wfd, &READY, 1);
    while ((n = read(rfd, &h, sizeof h)) == sizeof h && h.name[0])
    {
        snprintf(outname, sizeof outname, "%s.copy", h.name);
        int out = open(outname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out < 0)
        {
            fprintf(stderr, "Потомок: не удалось создать '%s': %s\n", outname, strerror(errno));
            status = 1;
        }
 
        for (off_t left = h.size; left > 0;)
        {
            ssize_t r = read(rfd, buf, left < BLOCK_SIZE ? left : BLOCK_SIZE);
            if (r <= 0)
                return 1;
            if (out >= 0 && write(out, buf, r) != r)
                status = 1;
            left -= r;
        }
        if (out >= 0)
            close(out);
        write(wfd, &READY, 1);
    }
    return n == sizeof h ? status : 1;
}
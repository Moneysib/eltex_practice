#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "transfer.h"
 
static int make_fifo(const char *name)
{
    struct stat st;
 
    if (mkfifo(name, 0666) == 0 || (errno == EEXIST && stat(name, &st) == 0 && S_ISFIFO(st.st_mode)))
        return 0;
    perror(name);
    return -1;
}
 
int main(int argc, char *argv[])
{
    const char *fifo = NULL;
    char ack[NAME_LEN + 8];
    int i = 1;
 
    if (argc > 2 && strcmp(argv[1], "-p") == 0)
    {
        fifo = argv[2];
        i = 3;
    }
    if (i >= argc || argv[i][0] == '-' || (fifo && strlen(fifo) >= NAME_LEN))
    {
        fprintf(stderr, "Использование: %s [-p имя_канала] файл [файл ...]\n", argv[0]);
        return 2;
    }
    signal(SIGPIPE, SIG_IGN);
 
    int p2c[2], c2p[2];
    if (fifo)
    {
        snprintf(ack, sizeof ack, "%s.ack", fifo);
        if (make_fifo(fifo) < 0 || make_fifo(ack) < 0)
            return 1;
    }
    else if (pipe(p2c) < 0 || pipe(c2p) < 0)
    {
        perror("pipe");
        return 1;
    }
 
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    if (pid == 0)
    {
        if (!fifo)
        {
            close(p2c[1]);
            close(c2p[0]);
        }
        int rfd = fifo ? open(fifo, O_RDONLY) : p2c[0];
        int wfd = fifo ? open(ack, O_WRONLY) : c2p[1];
        _exit(receive_files(rfd, wfd));
    }
 
    if (!fifo)
    {
        close(p2c[0]);
        close(c2p[1]);
    }
    int wfd = fifo ? open(fifo, O_WRONLY) : p2c[1];
    int rfd = fifo ? open(ack, O_RDONLY) : c2p[0];
    int code = 0;
 
    for (; i < argc; i++)
    {
        int r = send_file(wfd, rfd, argv[i]);
        if (r != 0)
            code = 1;
        if (r < 0)
            break;
    }
    if (i == argc && send_done(wfd, rfd) != 0)
        code = 1;
 
    close(wfd);
    close(rfd);
 
    int st;
    if (waitpid(pid, &st, 0) < 0 || !WIFEXITED(st) || WEXITSTATUS(st) != 0)
        code = 1;
    if (fifo)
    {
        unlink(fifo);
        unlink(ack);
    }
    return code;
}
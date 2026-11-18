#ifndef TRANSFER_H_
#define TRANSFER_H_
 
#define BLOCK_SIZE 4096
#define NAME_LEN   256
 
int send_file(int wfd, int rfd, const char *path);
int send_done(int wfd, int rfd);
 
int receive_files(int rfd, int wfd);
 
#endif
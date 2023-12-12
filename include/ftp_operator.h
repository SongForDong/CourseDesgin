#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "../include/network.h"


void ex();
void must();
void ls();
void cd_to(char* cd);
void download(char* get);
void upload(char* put);
void PWD();
void RM(char* path);
void MKDIR(char* path);
void RMD(char* path);

off_t get_filesize(char* filename);

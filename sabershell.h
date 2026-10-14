#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// 64(63 + NULL)
#define MAX_TOKEN_SIZE 64

// main.c
int judge_cmd(char **argv, int mode);
int handle_redirect(char **argv);
void executor (char **argv);

// parse.c
char** buf_parse(char* buf);
void free_tokens(char** tokens);
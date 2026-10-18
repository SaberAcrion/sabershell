#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// 最大token数量64(63 + NULL)
#define MAX_TOKEN_SIZE 64


typedef struct struct_shell
{
    char    *input;
    int     interactive;
} s_shell;


// main.c
int judge_cmd(char **argv, int mode);
int handle_redirect(char **argv);
void executor (char **argv);

//init.c
int init_interactive(int argc, char **argv, s_shell *shell);

// parse.c
char** buf_parse(char* buf);
void free_tokens(char** tokens);
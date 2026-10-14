#include "sabershell.h"

char** buf_parse(char* buf)
{
    // malloc失败返回NULL 
    size_t buf_token = MAX_TOKEN_SIZE;
    char** tokens = malloc(buf_token * sizeof(char*));
    size_t position = 0;

    // 检查tokens 初始化tokens
    if (!tokens)
    {
        perror("Malloc");
        exit(1);
    }
    for (size_t i = 0; i < buf_token; i++) tokens[i] = NULL;

    // 循坏每一个字符并且处理 结束条件为buf[i]为\0
    size_t i = 0;
    while (buf[i] != '\0')
    {
        // 对一个字符的首次判断 直接跳过空白字符 并且判断是不是buf末尾的情况
        while (isspace((unsigned char)buf[i]) && buf[i] != '\0') i++;
        if (buf[i] == '\0') break;

        // 记录token起点
        size_t token_start = i;

        // 对非空白字符进行处理 isspace不判断\0
        // 找到token末尾
        while (!isspace((unsigned char)buf[i]) && buf[i] != '\0') i++;
        
        // token大小
        size_t token_length = i - token_start;

        // 检查tokens容量
        if (position >= MAX_TOKEN_SIZE - 1)
        {
            fprintf(stderr, "Too many tokens\n");
            free_tokens(tokens);
            exit(1);
        }

        // 为指针数组每个指针指向分配内存
        tokens[position] = malloc((token_length + 1) * sizeof(char));
        if(!tokens[position])
        {
            perror("Malloc");
            free_tokens(tokens);
            exit(1);
        }

        // 赋值每一个
        for (size_t j = 0; j < token_length; j++)
        {
            tokens[position][j] = buf[token_start + j];
        }

        // \0终止符
        tokens[position][token_length] = '\0';
        
        position++;
    }

    // NULL结束数组
    tokens[position] = NULL;
    return tokens;
}


// 释放指针数组或数组
void free_tokens(char** tokens)
{
    if (!tokens) return;
    for (size_t i = 0; tokens[i]; i++) free(tokens[i]);
    free (tokens);
}
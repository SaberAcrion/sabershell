#include "sabershell.h"

int main(int argc, char *argv[])
{   
    // 交互模式 1 或脚本模式 0
    int interactive = 1;

    // 第一个参数是程序名 第二个是要读取的脚本
    if(argc > 2)
    {
        fprintf(stderr, "At most 2 arguments\n");
        exit(1);
    }
    else if (argc == 2)
    {
        // stdin绑定到脚本
        if (freopen(argv[1], "r", stdin) == NULL)
        {
            perror("freopen");
            fprintf(stderr, "Exiting\n");
            exit(1);
        }
        interactive = 0;
    }

    // 缓冲接收 指针数组
    char *buf = NULL;
    size_t buf_size = 0;
    char **cmd_argv;

    while (1)
    {
        // 提示符
        if (interactive == 1)
        {
            printf("\x1b[1;34ms@ber\x1b[0m ");
        }
        fflush(stdout);

        // getline stdin获取 自动补\0 区分EOF和错误
        if (getline(&buf, &buf_size, stdin) == -1)
        {
            if (ferror(stdin)) perror("getline");            
            break;
        }
        
        // 注释处理
        // char *hash = strchr(buf, '#');
        // if (hash) *hash = '\0';

        // 拆分buf为cmd_argv NULL作为指针数组结束
        cmd_argv = buf_parse(buf);

        // 判断cmd_argv[0] 并且传入模式副本
        if (judge_cmd(cmd_argv, interactive) == 1)
        {
            free_tokens(cmd_argv);
            continue;
        }
        
        // 执行和释放指针数组
        executor(cmd_argv);
        free_tokens(cmd_argv);
    }

    // 释放geiline malloc
    free(buf);
    return 0;
}

// 内建命令或空命令等等返回 1
int judge_cmd(char **argv, int interactive)
{
    if (argv[0] == NULL)
    {
        if (interactive == 1) printf("\x1b[1;34ms@ber: .....\n\x1b[0m");
        return 1;
    }
    // exit进程 堆内存自动释放
    else if (strcmp(argv[0], "exit") == 0)
    {
        if (interactive == 1) printf("\x1b[1;34mExited, sabershell\n\x1b[0m");
        exit(0);
    }

    return 0;
}

// 识别单个">"并且FD1重定向到文件 简单重定向
// 只支持>后单个文件名
int handle_redirect(char **argv)
{
    for (int i = 0; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i], ">") == 0)
        {
            if (argv[i + 1] == NULL)
            {
                fprintf(stderr, "The file name is missing after >\n");
                return 1;
            }
            else if (argv[i + 2] != NULL)
            {
                fprintf(stderr, "The file name can only be one\n");
                return 1;
            }

            int fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("open");
                return 1;
            }

            //重定向 FD1->terminal 变成FD1->argv[i + 1] 关闭原来fd
            dup2(fd, STDOUT_FILENO);
            close(fd);

            //设空 i之前的为命令 打断for循环
            argv[i] = NULL;
            break;
        }
    }
    return 0;
}

void executor(char **argv)
{
    // fork 一个父进程一个子进程 返回值区分 声明pid类型是pid_t
    // 这时就有2个进程2个pid返回值 if来让2个进程走不同状态
    pid_t pid = fork();

    // 父进程返回值>0并且返回值是子进程PID 子进程返回值为0
    if (pid > 0)
    {
        // 父进程 wait(NULL)只等待子进程结束不关心结束状态
        wait(NULL);
    }
    else if (pid == 0)
    {
        if (handle_redirect(argv) != 0) exit(0);

        // 子进程 execvp来执行新的程序
        execvp(argv[0], argv);
        // 打印错误到stderr
        perror("execvp");
        exit(1);
    }
    else
    {
        // 打印错误到stderr
        perror("fork");
        exit(1);
    }
}
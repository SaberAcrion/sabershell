#include "sabershell.h"

int main(int argc, char *argv[])
{   

    // 初始化shell结构体和判断交互
    s_shell shell = {NULL};
    if (init_interactive(argc, argv, &shell) != 0) return 1;

    // 缓冲接收 指针数组
    size_t input_size = 0;
    char **cmd_argv;

    while (1)
    {
        // 提示符
        if (shell.interactive == 1) printf("s@ber ");
        
        fflush(stdout);

        // getline自动扩容自动补\0 无需重复free
        // ferror区分EOF和错误(都是返回-1) 
        if (getline(&shell.input, &input_size, stdin) == -1)
        {
            if (ferror(stdin)) perror("getline");            
            break;
        }
        
        // 注释处理
        // char *hash = strchr(buf, '#');
        // if (hash) *hash = '\0';

        // 拆分buf为cmd_argv NULL作为指针数组结束
        cmd_argv = buf_parse(shell.input);

        // 判断cmd_argv[0] 并且传入模式副本
        if (judge_cmd(cmd_argv, shell.interactive) == 1)
        {
            free_tokens(cmd_argv);
            continue;
        }
        
        // 执行和释放指针数组
        executor(cmd_argv);
        free_tokens(cmd_argv);
    }

    // 释放geiline
    free(shell.input);
    return 0;
}

// 内建命令或空命令等等返回 1
int judge_cmd(char **argv, int interactive)
{
    if (argv[0] == NULL)
    {
        if (interactive == 1) printf("s@ber:.....\n");
        return 1;
    }
    // exit进程 堆内存自动释放
    else if (strcmp(argv[0], "exit") == 0)
    {
        if (interactive == 1) printf("s@ber:exiting\n");
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
                fprintf(stderr, "the file name is missing after >\n");
                return 1;
            }
            else if (argv[i + 2] != NULL)
            {
                fprintf(stderr, "the file name can only be one\n");
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
    pid_t pid = fork();
    int child_status;

    // 父进程返回值>0并且返回值是子进程PID 子进程返回值为0
    // fork后有独立的地址空间 _exit来防止父进程缓冲影响子进程
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        if (handle_redirect(argv) != 0) _exit(1);

        // 子进程 execvp来执行新的程序
        execvp(argv[0], argv);
        // 打印错误到stderr
        perror(argv[0]);
        _exit(1);
    }
    else
    {
        // 0表示阻塞等待 返回状态写入status
        waitpid(pid, &child_status, 0);
    }
}
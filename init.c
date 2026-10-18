#include "sabershell.h"

int init_interactive(int argc, char **argv, s_shell *shell)
{
    if(argc > 2)
    {
        fprintf(stderr, "s@ber: too many arguments\n");
        return 1;
    }
    if (argc == 2)
    {
        // 重定向STDIN流到argv[1]
        if (freopen(argv[1], "r", stdin) == NULL)
        {
            perror(argv[1]);
            return 1;
        }
    }
    // 交互或非交互
    shell->interactive = isatty(STDIN_FILENO);
    return 0;
}
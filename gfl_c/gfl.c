#include "gfl.h"

int main(int argc, char **argv)
{
    if (ERROR == option_parse(argc, argv))
        return 1;
    debug_args();

    char **dir = get_dirpaths();
    while (0 != strcmp("#", *dir))
    {
        traversal_dir(*dir++);
    }

    return 0;
}

void debug_args()
{
    debug_var(1, "%d", getopt_filetype(OFT_REG));
    debug_var(1, "%d", getopt_filetype(OFT_DIR));
    debug_var(1, "%d", getopt_filetype(OFT_SYM));
    debug_var(1, "%d", getopt_reverse());
    debug_var(1, "%d", getopt_depth());
    debug_var(1, "%d", getopt_group());

    char **dir = get_dirpaths();
    while (0 != strcmp("#", *dir))
    {
        debug_var(1, "%s", *dir);
        dir++;
    }
}

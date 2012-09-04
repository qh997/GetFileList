#include "gfl.h"

int main(int argc, char **argv)
{
    if (ERROR == option_parse(argc, argv))
        return 1;
    debug_args();

    char **dir = get_dirpaths();

    while (0 != strcmp("#", *dir))
    {
        DirBox *dirBox = (DirBox *)malloc(sizeof(DirBox));
        InitDirBox(dirBox, *dir);
        FillDirBox(*dir++, dirBox);

        ForEachChild(dirBox, "", 1, PrintBabys);
    }

    return 0;
}

void debug_args()
{
    DEBUG_VAR(1, "%d", getopt_filetype(OFT_REG));
    DEBUG_VAR(1, "%d", getopt_filetype(OFT_DIR));
    DEBUG_VAR(1, "%d", getopt_filetype(OFT_SYM));
    DEBUG_VAR(1, "%d", getopt_reverse());
    DEBUG_VAR(1, "%d", getopt_depth());
    DEBUG_VAR(1, "%d", getopt_group());

    char **dir = get_dirpaths();
    while (0 != strcmp("#", *dir))
    {
        DEBUG_VAR(1, "%s", *dir);
        dir++;
    }
}

void PrintBabys(DirBox *dbox, char *dpath, int dep)
{
    DEBUG_VAR(2, "%d", dbox->child_count);
    DEBUG_VAR(2, "%d", dep);

    if (getopt_depth() < dep) return;

    FileBox *baby = dbox->baby;
    while (NULL != baby)
    {
        DEBUG_VAR(2, "%s", dpath);
        DEBUG_VAR(2, "%s", baby->fname);
        printf("%s", dpath);
        printf("%s\n", baby->fname);
        baby = baby->rear;
    }
}

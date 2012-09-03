#include "options.h"

static int boolOptions = 0;
static int depth = 0;
static OptSortBy sortBy = OSB_PATH;
static OptOutput output = OOP_REL;
static int debugLevel = 0;
static char **dirpaths = NULL;

static void remove_last_separator(char *dir);

Status option_parse(int argc, char **argv)
{
    const char *shortopts = "ht:d:rs:o:g";
    const struct option longopts[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"type", required_argument, NULL, 't'},
        {"depth", required_argument, NULL, 'd'},
        {"reverse", no_argument, NULL, 'r'},
        {"sort-by", required_argument, NULL, 's'},
        {"output", required_argument, NULL, 'o'},
        {"group", no_argument, NULL, 'g'},
        {"debug", required_argument, NULL, 'b'},
        {0, 0, 0, 0}
    };

    while (TRUE)
    {
        int opt_index = 0;
        int current_option = getopt_long(argc, argv, shortopts, longopts, &opt_index);

        if (-1 == current_option)
        {
            init_filetype();
            break;
        }

        switch (current_option)
        {
            case 'h':
                usage(TRUE);
                exit(0);
                break;

            case 't':
                if (ERROR == setopt_filetype(optarg, FALSE))
                {
                    fprintf(stderr, "Value '%s' invalid for option -t/--type (choose 'reg', 'dir', 'sym')\n", optarg);
                    usage(FALSE);
                    return ERROR;
                }
                break;

            case 'd':
                if (ERROR == setopt_depth(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option -d/--depth (number expected)\n", optarg);
                    usage(FALSE);
                    return ERROR;
                }
                break;

            case 'r':
                setopt_reverse(TRUE);
                break;

            case 's':
                if (ERROR == setopt_sortby(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option -s/--sort-by", optarg);
                    fprintf(stderr, " (choose 'path', 'file', 'size', 'date', 'null')\n");
                    usage(FALSE);
                    return ERROR;
                }
                break;

            case 'o':
                if (ERROR == setopt_output(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option -o/--output", optarg);
                    fprintf(stderr, " (choose 'abs', 'rel', 'bsn')\n");
                    usage(FALSE);
                    return ERROR;
                }
                break;
                break;

            case 'g':
                setopt_group(TRUE);
                break;

            case 'b':
                if (ERROR == setopt_debuglevel(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option --debug (number expected)\n", optarg);
                    usage(FALSE);
                    return ERROR;
                }
                break;

            case '?':
                usage(FALSE);
            default:
                return ERROR;
        }
    }

    if (optind < argc)
    {
        dirpaths = (char **)malloc(sizeof(char *) * (argc - optind + 1));
        char **paths = dirpaths;
        for (int i = optind; i < argc; i++)
        {
            *paths = (char *)malloc(strlen(*(argv + i)) + 1);
            strcpy(*paths, *(argv + i));
            remove_last_separator(*paths);
            paths++;
        }
        *paths = (char *)malloc(2);
        strcpy(*paths, "#");
    }
    else
    {
        char crt_dir[FILENAME_MAX];
        getcwd(crt_dir, FILENAME_MAX);
        dirpaths = (char **)malloc(sizeof(char *) * 2);
        *dirpaths = (char *)malloc(strlen(crt_dir) + 1);
        strcpy(*dirpaths, crt_dir);
        remove_last_separator(*dirpaths);

        *(dirpaths + 1) = (char *)malloc(2);
        strcpy(*(dirpaths + 1), "#");
    }

    return OK;
}

bool getopt_filetype(const OptFileType value){return boolOptions & value;}

Status setopt_filetype(const char *value, const bool unset)
{
    if (unset)
    {
        if (0 == strcmp(value, "reg"))
            boolOptions &= ~OFT_REG;
        else if(0 == strcmp(value, "dir"))
            boolOptions &= ~OFT_DIR;
        else if(0 == strcmp(value, "sym"))
            boolOptions &= ~OFT_SYM;
        else
            return ERROR;
    }
    else
    {
        if (0 == strcmp(value, "reg"))
            boolOptions |= OFT_REG;
        else if(0 == strcmp(value, "dir"))
            boolOptions |= OFT_DIR;
        else if(0 == strcmp(value, "sym"))
            boolOptions |= OFT_SYM;
        else
            return ERROR;
    }

    return OK;
}

void init_filetype()
{
    if (!getopt_filetype(OFT_REG) && !getopt_filetype(OFT_DIR) && !getopt_filetype(OFT_SYM))
        setopt_filetype("reg", FALSE);
}

int getopt_depth(){return depth;}

Status setopt_depth(const char *value)
{
    int ivalue;
    if (ERROR == string_to_int(value, &ivalue))
        return ERROR;
    else
        depth = ivalue;
}

bool getopt_reverse(){return boolOptions & OPT_REVE;}

Status setopt_reverse(const bool value)
{
    if (value)
        boolOptions |= OPT_REVE;
    else
        boolOptions &= ~OPT_REVE;

    return OK;
}

bool getopt_sortby(const OptSortBy value){return value == sortBy ? TRUE : FALSE;}

Status setopt_sortby(const char *value)
{
    if (0 == strcmp(value, "path"))
        sortBy = OSB_PATH;
    else if (0 == strcmp(value, "file"))
        sortBy = OSB_FILE;
    else if (0 == strcmp(value, "size"))
        sortBy = OSB_SIZE;
    else if (0 == strcmp(value, "date"))
        sortBy = OSB_DATE;
    else if (0 == strcmp(value, "null"))
        sortBy = OSB_NULL;
    else
        return ERROR;

    return OK;
}

bool getopt_output(const OptOutput value){return value == output ? TRUE : FALSE;}

Status setopt_output(const char *value)
{
    if (0 == strcmp(value, "abs"))
        output = OOP_ABS;
    else if (0 == strcmp(value, "rel"))
        output = OOP_REL;
    else if (0 == strcmp(value, "bsn"))
        output = OOP_BSN;
    else
        return ERROR;

    return OK;
}

bool getopt_group(){return boolOptions & OPT_GRUP;}

Status setopt_group(const bool value)
{
    if (value)
        boolOptions |= OPT_GRUP;
    else
        boolOptions &= ~OPT_GRUP;

    return OK;
}

bool is_debug(int lev){return debugLevel >= lev ? TRUE : FALSE;}

Status setopt_debuglevel(const char *value)
{
    int ivalue;
    if (ERROR == string_to_int(value, &ivalue))
        return ERROR;
    else
        debugLevel = ivalue;

    return OK;
}

char **get_dirpaths()
{
    return dirpaths;
}

void usage(bool verbose)
{
    printf("\
Usage:\n\
    gfl.pl [options] [path ...]\n\n\
      Options:\n\
      -h, --help\n\
      -t {reg|dir|sym}, --type {reg|dir|sym}\n\
      -d <depth>, --depth <depth>\n\
      -r, --reverse\n\
      -s {path|file|size|date}, --sort-by {path|file|size|date|null}\n\
      -o {abs|rel|bsn}, --output {abs|rel|bsn}\n\
      -g, --group\n\
      --debug <debug-level>\n\
      -v, --version\n"
    );

    if (verbose)
    {
        printf("\n\
Options:\n\
    -h, --help\n\
            Show this help message and exit.\n\n\
\n\
    -t {reg|dir|sym}, --type {reg|dir|sym}\n\
            Specify the file type of the searching.\n\
\n\
            {reg}   Regular file.\n\
            {dir}   Directory.\n\
            {sym}   Symbolic link.\n\
\n\
    -d <depth>, --depth <depth>\n\
            Specify the depth of the searching.\n\
\n\
    -r, --reverse\n\
            Reverse the result.\n\
\n\
    -s {path|file|size|date}, --sort-by {path|file|size|date|null}\n\
            Specify the key of sort.\n\
\n\
            {path}  Sort by path name.\n\
            {file}  Sort by file name.\n\
            {size}  Sort by size of file.\n\
            {date}  Sort by time of last modification.\n\
            {null}  Use default order.\n\
\n\
    -o {abs|rel|bsn}, --optput {abs|rel|bsn}\n\
            Specify the key of sort.\n\
\n\
            {abs}   Absolute path.\n\
            {rel}   Relative path.\n\
            {bsn}   File name only.\n\
\n\
    -g, --group\n\
            Frouping the result.\n\
\n\
    --debug <debug-level>\n\
            Set the debug level.\n\
\n\
    -v, --version\n\
            Display the version of this program and exit.\n"
        );
    }
}

static void remove_last_separator(char *dir)
{
    int dir_length = strlen(dir);
    debug_var(3, "%d", dir_length);
    if (0 < dir_length && '/' == *(dir + dir_length - 1))
    {
        *(dir + dir_length - 1) = '\0';
    }
}

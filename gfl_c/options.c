#include "options.h"

static int _s_bool_settings = 0;
static int _s_depth = 0;
static EN_OptSortBy _s_sort_by = OSB_PATH;
static EN_OptOutput _s_output = OOP_REL;
static char **_s_dirpaths = NULL;

static Status _setopt_filetype(const char *value, const bool unset);
static Status _setopt_depth(const char *value);
static Status _setopt_reverse(const bool value);
static Status _setopt_sortby(const char *value);
static Status _setopt_output(const char *value);
static Status _setopt_group(const bool value);
static void _remove_trailing_separator(char *dir);
static void _init_filetype();

Status parse_args(int argc, char **argv)
{
    const char *short_opts = "ht:d:rs:o:g";
    const struct option long_opts[] =
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

    while (true)
    {
        int opt_index = 0;
        int current_option = getopt_long(argc, argv, short_opts, long_opts, &opt_index);

        if (-1 == current_option)
        {
            _init_filetype();
            break;
        }

        switch (current_option)
        {
            case 'h':
                usage(true);
                exit(0);
                break;

            case 't':
                if (ERROR == _setopt_filetype(optarg, false))
                {
                    fprintf(stderr, "Value '%s' invalid for option -t/--type (choose 'reg', 'dir', 'sym')\n", optarg);
                    usage(false);
                    return ERROR;
                }
                break;

            case 'd':
                if (ERROR == _setopt_depth(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option -d/--depth (number expected)\n", optarg);
                    usage(false);
                    return ERROR;
                }
                break;

            case 'r':
                _setopt_reverse(true);
                break;

            case 's':
                if (ERROR == _setopt_sortby(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option -s/--sort-by", optarg);
                    fprintf(stderr, " (choose 'path', 'file', 'size', 'date', 'null')\n");
                    usage(false);
                    return ERROR;
                }
                break;

            case 'o':
                if (ERROR == _setopt_output(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option -o/--output", optarg);
                    fprintf(stderr, " (choose 'abs', 'rel', 'bsn')\n");
                    usage(false);
                    return ERROR;
                }
                break;
                break;

            case 'g':
                _setopt_group(true);
                break;

            case 'b':
                if (ERROR == set_debug_level(optarg))
                {
                    fprintf(stderr, "Value '%s' invalid for option --debug (number expected)\n", optarg);
                    usage(false);
                    return ERROR;
                }
                break;

            case '?':
                usage(false);
            default:
                return ERROR;
        }
    }

    if (optind < argc)
    {
        _s_dirpaths = (char **)malloc(sizeof(char *) * (argc - optind + 1));
        char **paths = _s_dirpaths;
        for (int i = optind; i < argc; i++)
        {
            *paths = (char *)malloc(strlen(*(argv + i)) + 1);
            strcpy(*paths, *(argv + i));
            _remove_trailing_separator(*paths);
            paths++;
        }
        *paths = (char *)malloc(2);
        strcpy(*paths, "\0");
    }
    else
    {
        char crt_dir[FILENAME_MAX];
        getcwd(crt_dir, FILENAME_MAX);
        _s_dirpaths = (char **)malloc(sizeof(char *) * 2);
        *_s_dirpaths = (char *)malloc(strlen(crt_dir) + 1);
        strcpy(*_s_dirpaths, crt_dir);
        _remove_trailing_separator(*_s_dirpaths);

        *(_s_dirpaths + 1) = (char *)malloc(2);
        strcpy(*(_s_dirpaths + 1), "\0");
    }

    return OK;
}

bool getopt_filetype(const EN_OptFileType value) {return _s_bool_settings & value;}

int getopt_depth() {return _s_depth;}

bool getopt_reverse() {return _s_bool_settings & OPT_REVE;}

bool getopt_sortby(const EN_OptSortBy value) {return value == _s_sort_by ? true : false;}

bool getopt_output(const EN_OptOutput value) {return value == _s_output ? true : false;}

bool getopt_group() {return _s_bool_settings & OPT_GRUP;}

char **get_dirpaths()
{
    return _s_dirpaths;
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

/* Private functions */

static Status _setopt_filetype(const char *value, const bool unset)
{
    if (unset)
    {
        if (0 == strcmp(value, "reg"))
            _s_bool_settings &= ~OFT_REG;
        else if(0 == strcmp(value, "dir"))
            _s_bool_settings &= ~OFT_DIR;
        else if(0 == strcmp(value, "sym"))
            _s_bool_settings &= ~OFT_SYM;
        else
            return ERROR;
    }
    else
    {
        if (0 == strcmp(value, "reg"))
            _s_bool_settings |= OFT_REG;
        else if(0 == strcmp(value, "dir"))
            _s_bool_settings |= OFT_DIR;
        else if(0 == strcmp(value, "sym"))
            _s_bool_settings |= OFT_SYM;
        else
            return ERROR;
    }

    return OK;
}

static Status _setopt_depth(const char *value)
{
    int ivalue;
    if (ERROR == string_to_int(value, &ivalue))
        return ERROR;
    else
        _s_depth = ivalue;

    return OK;
}

static Status _setopt_reverse(const bool value)
{
    if (value)
        _s_bool_settings |= OPT_REVE;
    else
        _s_bool_settings &= ~OPT_REVE;

    return OK;
}

static Status _setopt_sortby(const char *value)
{
    if (0 == strcmp(value, "path"))
        _s_sort_by = OSB_PATH;
    else if (0 == strcmp(value, "file"))
        _s_sort_by = OSB_FILE;
    else if (0 == strcmp(value, "size"))
        _s_sort_by = OSB_SIZE;
    else if (0 == strcmp(value, "date"))
        _s_sort_by = OSB_DATE;
    else if (0 == strcmp(value, "null"))
        _s_sort_by = OSB_NULL;
    else
        return ERROR;

    return OK;
}

static Status _setopt_output(const char *value)
{
    if (0 == strcmp(value, "abs"))
        _s_output = OOP_ABS;
    else if (0 == strcmp(value, "rel"))
        _s_output = OOP_REL;
    else if (0 == strcmp(value, "bsn"))
        _s_output = OOP_BSN;
    else
        return ERROR;

    return OK;
}

static Status _setopt_group(const bool value)
{
    if (value)
        _s_bool_settings |= OPT_GRUP;
    else
        _s_bool_settings &= ~OPT_GRUP;

    return OK;
}

static void _init_filetype()
{
    if (!getopt_filetype(OFT_REG) && !getopt_filetype(OFT_DIR) && !getopt_filetype(OFT_SYM))
        _setopt_filetype("reg", false);
}

static void _remove_trailing_separator(char *dir)
{
    int dir_length = strlen(dir);
    DEBUG_VAR(3, "%d", dir_length);
    if (0 < dir_length && PATHSEPC == *(dir + dir_length - 1))
    {
        *(dir + dir_length - 1) = '\0';
    }
}

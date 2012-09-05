#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "common.h"

#define OPT_HELP (1 << 0)
#define OPT_REVE (1 << 1)
#define OPT_GRUP (1 << 2)

typedef enum
{
    OFT_REG = 1 << 3,
    OFT_DIR = 1 << 4,
    OFT_SYM = 1 << 5,
} OptFileType;

typedef enum
{
    OSB_PATH = 1,
    OSB_FILE,
    OSB_SIZE,
    OSB_DATE,
    OSB_NULL,
} OptSortBy;

typedef enum
{
    OOP_ABS = 1,
    OOP_REL,
    OOP_BSN,
} OptOutput;

Status option_parse(int argc, char **argv);

bool getopt_filetype(const OptFileType value);
Status setopt_filetype(const char *value, const bool unset);
void init_filetype();

int getopt_depth();
Status setopt_depth(const char *value);

bool getopt_reverse();
Status setopt_reverse(const bool value);

bool getopt_sortby(const OptSortBy value);
Status setopt_sortby(const char *value);

bool getopt_output(const OptOutput value);
Status setopt_output(const char *value);

bool getopt_group();
Status setopt_group(const bool value);

char **get_dirpaths();

void usage(bool verbose);

#endif //_OPTIONS_H

/*struct option {
    const char *name; //name表示的是长参数名
    int has_arg; //has_arg有3个值
                 //no_argument(或者是0)，表示该参数后面不跟参数值
                 //required_argument(或者是1),表示该参数后面一定要跟个参数值
                 //optional_argument(或者是2),表示该参数后面可以跟，也可以不跟参数值
    int *flag; //用来决定，getopt_long()的返回值到底是什么。如果flag是null，则函数会返回与该项option匹配的val值
    int val; //和flag联合决定返回值
}*/

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

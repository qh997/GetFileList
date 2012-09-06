#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define OK (1)
#define ERROR (0)
#define INFEASIBLE (-1)
#define OVERFLOW (-2)

#define PATHSEPS "/"
#define PATHSEPC '/'

#define show_var(fmt, var) fprintf(stderr, "%s = "fmt"\n", #var, var)
#define DEBUG_VAR(lev, fmt, var)\
    if (is_debug(lev))\
    {\
        char title[100] = {'\0'};\
        debug_head(lev, title);\
        fprintf(stderr, "%s %s = "fmt"\n", title, #var, var);\
    }
#define DEBUG_LOG(lev, fmt, msg)\
    if (is_debug(lev))\
    {\
        char title[100] = {'\0'};\
        debug_head(lev, title);\
        fprintf(stderr, "%s "fmt"\n", title, msg);\
    }

typedef int Status;

Status set_debug_level(const char *value);
bool is_debug(int lev);
void debug_head(int lev, char *title);
Status string_to_int(const char *nprt, int *num);

#endif //__COMMON_H__

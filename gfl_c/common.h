#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define TRUE (1)
#define FALSE (0)
#define OK (1)
#define ERROR (0)
#define INFEASIBLE (-1)
#define OVERFLOW (-2)
typedef int Status;

#define show_var(fmt, var) fprintf(stderr, "%s = "fmt"\n", #var, var)
#define debug_var(lev, fmt, var)\
    if (is_debug(lev))\
    {\
        char title[100] = {'\0'};\
        debug_title(lev, title);\
        fprintf(stderr, "%s %s = "fmt"\n", title, #var, var);\
    }

Status string_to_int(const char *nprt, int *num);
void debug_title(int lev, char *title);

#endif //_COMMON_H

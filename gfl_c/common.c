#include "common.h"

static int _s_debug_level = 0;

Status set_debug_level(const char *value)
{
    int ivalue;
    if (ERROR == string_to_int(value, &ivalue))
        return ERROR;
    else
        _s_debug_level = ivalue;

    return OK;
}

bool is_debug(int lev) {return _s_debug_level >= lev ? true : false;}

void debug_head(int lev, char *title)
{
    char buf[50] = {'\0'};
    time_t t0;
    time(&t0);
    strftime(buf, sizeof buf, "%X", localtime(&t0));

    strcpy(title, "[DebugMessage ");
    strcat(title, buf);
    sprintf(buf, " %d]", lev);
    strcat(title, buf);
}

Status string_to_int(const char *nprt, int *num)
{
    char c;
    int i = 0;
    *num = 0;
    while (c = *(nprt + i))
    {
        if (isdigit(c))
        {
            *num *= 10;
            *num += c - '0';
        }
        else
            return ERROR;

        i++;
    }

    return OK;
}

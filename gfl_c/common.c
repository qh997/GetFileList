#include "common.h"

static int debugLevel = 0;

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
        {
            return ERROR;
        }

        i++;
    }

    return OK;
}

void debug_title(int lev, char *title)
{
    char buf[50] = {'\0'};

    strcpy(title, "[DebugMessage ");
    time_t t0;
    time(&t0);
    strftime(buf, sizeof buf, "%X", localtime(&t0));
    strcat(title, buf);
    sprintf(buf, " %d]", lev);
    strcat(title, buf);
}

#ifndef __GFL_H__
#define __GFL_H__

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "options.h"
#include "dirbox.h"
#include "queuelist.h"

void debug_args();
void fill_result_list(ST_DirBox *dbox, char *dpath, int dep);

#endif //__GFL_H__

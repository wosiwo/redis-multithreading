//
// Created by onceme on 2019/3/24.
//

#ifndef REDIS_3_0_ANNOTATED_ATOM_LIST_H
#define REDIS_3_0_ANNOTATED_ATOM_LIST_H

#endif //REDIS_3_0_ANNOTATED_ATOM_LIST_H

#include <stdbool.h>
#include "adlist.h"

list *atomListCreate(void);
list *atomListAddNodeTail(list *list, void *value);

void *atomListPop(list *list);

int incListLen(list *list,int inc);
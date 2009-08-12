#ifndef _PREFS_H_
#define _PREFS_H_

#include <stdio.h>

typedef struct {
    int (*read)(char *str, void **value);
    int (*write)(FILE *fp, void *value);
} type_t;

type_t pref_get_type(char *name);

typedef struct {
    char *type;
    char *name;  
    void *value;      
} var_t;


#endif

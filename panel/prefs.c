
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "prefs.h"

static GHashTable *names;
static GHashTable *types;

/* string type */
static int 
type_str_read(char *str, void **value)
{
    if (str && *str) {
        *value = gstrdup(str);
        return 1;
    }
    return 0;
}

static int
type_str_write(FILE *fp, void *value)
{
    fprintf(fp, "%s", value);
    return 1;
}

static type_t type_str_t = {
    .name  = "string",
    .read  = type_str_read,
    .write = type_str_write,
};

static void
pref_init()
{
    names = g_hash_table_new(g_str_hash, g_str_equal);
    types = g_hash_table_new(g_str_hash, g_str_equal);
}

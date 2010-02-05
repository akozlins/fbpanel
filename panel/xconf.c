#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

#include "xconf.h"
#include "profile.h"
#include "dbg.h"


/* Creates new xconf node */
xconf *xconf_new(gchar *name, gchar *value)
{
    xconf *x;

    x = g_new0(xconf, 1);
    x->name = g_strdup(name);
    x->value = g_strdup(value);
    return x;
}

/* Adss new son node */
void xconf_link(xconf *parent, xconf *son)
{
    son->parent = parent;
    /* appending requires traversing all list to the end, which is not
     * efficient, but for v 1.0 it's ok*/
    parent->sons = g_slist_append(parent->sons, son);
}

/* Unlinks node from its parent, if any */
void xconf_unlink(xconf *x)
{
    if (x && x->parent)
    {
        x->parent->sons = g_slist_remove(x->parent->sons, x);
        x->parent = NULL;
    }
}

/* Unlinks node and deletes it and its sub-tree */
void xconf_del(xconf *x, gboolean sons_only)
{
    GSList *s;
    xconf *x2;
    
    DBG("%s %s\n", x->name, x->value);
    for (s = x->sons; s; s = g_slist_delete_link(s, s))
    {
        x2 = s->data;
        x2->parent = NULL;
        xconf_del(x2, FALSE);
    }
    x->sons = NULL;
    if (!sons_only)
    {
        g_free(x->name);
        g_free(x->value);
        xconf_unlink(x);
        g_free(x);
    }
}

void xconf_set_value(xconf *x, gchar *value)
{
    xconf_del(x, TRUE);
    g_free(x->value);
    x->value = g_strdup(value);
  
}

gchar *xconf_get_value(xconf *x)
{
    return x->value;
}

void xconf_prn(FILE *fp, xconf *x, int n, gboolean sons_only)
{
    int i;
    GSList *s;
    xconf *x2;

    if (!sons_only)
    {
        for (i = 0; i < n; i++)
            printf("    ");
        fprintf(fp, "%s", x->name);
        if (x->value)
            fprintf(fp, " = %s\n", x->value);
        else
            fprintf(fp, " {\n");
        n++;
    }
    for (s = x->sons; s; s = g_slist_next(s))
    {
        x2 = s->data;
        xconf_prn(fp, x2, n, FALSE);
    }
    if (!sons_only && !x->value)
    {
        n--;
        for (i = 0; i < n; i++)
            fprintf(fp, "    ");
        fprintf(fp, "}\n");
    }

}

xconf *xconf_find(xconf *x, gchar *name, int no)
{
    GSList *s;
    xconf *x2;

    if (!x)
        return NULL;
    for (s = x->sons; s; s = g_slist_next(s))
    {
        x2 = s->data;
        if (!strcasecmp(x2->name, name))
        {
            if (!no)
                return x2;
            no--;
        }
    }
    return NULL;
}


void xconf_get_str(xconf *x, gchar **val)
{
    if (x && x->value)
        *val = x->value;
}


void xconf_get_int(xconf *x, int *val)
{
    gchar *s;

    if (!x)
        return;
    s = xconf_get_value(x);
    if (!s)
        return;
    *val = strtol(s, NULL, 0);
}

void xconf_get_enum(xconf *x, int *val, xconf_enum *p)
{
    gchar *s;

    if (!x)
        return;
    s = xconf_get_value(x);
    if (!s)
        return;
    while (p && p->str)
    {
        DBG("cmp %s %s\n", p->str, s);
        if (!strcasecmp(p->str, s))
        {
            *val = p->num;
            return;
        }
        p++;
    }
}




xconf *read_block(FILE *fp, gchar *name)
{
    line s;
    xconf *x, *xs;

    x = xconf_new(name, NULL);
    while (get_line(fp, &s) != LINE_NONE)
    {
        if (s.type == LINE_BLOCK_START)
        {
            xs = read_block(fp, s.t[0]);
            xconf_link(x, xs);
        }
        else if (s.type == LINE_BLOCK_END)
            break;
        else if (s.type == LINE_VAR)
        {
            xs = xconf_new(s.t[0], s.t[1]);
            xconf_link(x, xs);
        }
        else
        {
            printf("syntax error\n");
            exit(1);
        }
    }
    return x;
}

xconf *xconf_new_from_profile(gchar *profile)
{
    FILE *fp = get_profile_file(profile, "r");
    xconf *ret = NULL;
    
    if (fp)
    {
        ret = read_block(fp, profile);
        fclose(fp);
    }
    return ret;
}

void xconf_save_to_profile(gchar *profile, xconf *xc)
{
    FILE *fp = get_profile_file(profile, "w");

    if (fp)
    {
        xconf_prn(fp, xc, 0, TRUE);
        fclose(fp);
    }
}

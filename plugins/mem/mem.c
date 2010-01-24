#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUGPRN
#include "dbg.h"


typedef struct
{
    plugin_instance plugin;
    GtkWidget *mem_pb;
    GtkWidget *swap_pb;
    GtkWidget *box;
    int timer;
    int show_swap;
} mem_priv;

typedef struct
{
    char *name;
    gdouble val;
    int valid;
} mem_type_t;


#if defined __linux__
#undef MT_ADD
#define MT_ADD(x) MT_ ## x,
enum {
#include "mt.h"
    MT_NUM
};

#undef MT_ADD
#define MT_ADD(x) { #x, 0, 0 },
mem_type_t mt[] =
{
#include "mt.h"
};

static gboolean
mt_match(char *buf, mem_type_t *m)
{
    gulong val;
    int len;

    len = strlen(m->name);
    if (strncmp(buf, m->name, len))
        return FALSE;
    if (sscanf(buf + len + 1, "%lu", &val) != 1)
        return FALSE;
    m->val = (gdouble) val;
    m->valid = 1;
    DBG("%s: %lu\n", m->name, val);
    return TRUE;
}

static void
mem_usage(gdouble *mu, gdouble *su)
{
    FILE *fp;
    char buf[160];
    int i;

    *mu = *su = 0;
    fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return;
    for (i = 0; i < MT_NUM; i++)
    {
        mt[i].valid = 0;
        mt[i].val = 0;
    }

    while ((fgets(buf, sizeof(buf), fp)) != NULL)
    {
        for (i = 0; i < MT_NUM; i++)
        {
            if (!mt[i].valid && mt_match(buf, mt + i))
                break;
        }
    }
    fclose(fp);
  
    if (mt[MT_MemTotal].val == 0.0)
    {
        DBG("%s is zero\n", mt[MT_MemTotal].name);
        return;
    }
    *mu = 1 - ((mt[MT_MemFree].val + mt[MT_Buffers].val + mt[MT_Cached].val +
                    mt[MT_Slab].val) / mt[MT_MemTotal].val);
    *su = 1 - (mt[MT_SwapFree].val / mt[MT_SwapTotal].val);
}
#else
static void
mem_usage(gdouble *mu, gdouble *su)
{
    *mu = *su = 0;
}
#endif

static void
mem_update(mem_priv *mem)
{
    gdouble mu, su;
    char str[40];
    ENTER;
    mem_usage(&mu, &su);
    g_snprintf(str, sizeof(str), "<b>Mem: %d%%</b>\n<b>Swap: %d%%</b>",
            (int)(mu * 100), (int) (su * 100));
    gtk_widget_set_tooltip_markup(mem->plugin.pwid, str);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(mem->mem_pb), mu);
    if (mem->show_swap)
        gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(mem->swap_pb), su);
}


static void
mem_destructor(plugin_instance *p)
{
    mem_priv *mem = (mem_priv *)p;

    ENTER;
    if (mem->timer)
        g_source_remove(mem->timer);
    gtk_widget_destroy(mem->box);
    RET();
}

static int
mem_constructor(plugin_instance *p)
{
    mem_priv *mem;
    gint w, h;
    GtkProgressBarOrientation o;
    line s;
    
    ENTER;
    mem = (mem_priv *) p;
    while (get_line(p->fp, &s) != LINE_BLOCK_END)
    {
        if (s.type == LINE_NONE)
        {
            ERR( "space: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR)
        {
            if (!g_ascii_strcasecmp(s.t[0], "ShowSwap"))
                mem->show_swap = str2num(bool_pair, s.t[1], 1);
            else
            {
                ERR("mem: unknown var %s\n", s.t[0]);
                goto error;
            }
        }
        else
        {
            ERR("menu: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    mem->box = p->panel->my_box_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (mem->box), 0);

    if (p->panel->orientation == ORIENT_HORIZ)
    {
        o = GTK_PROGRESS_BOTTOM_TO_TOP;
        w = 9;
        h = 0;
    }
    else
    {
        o = GTK_PROGRESS_LEFT_TO_RIGHT;
        w = 0;
        h = 9;
    }  
    mem->mem_pb = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(mem->box), mem->mem_pb, FALSE, FALSE, 0);
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(mem->mem_pb), o);
    gtk_widget_set_size_request(mem->mem_pb, w, h);

    if (mem->show_swap)
    {
        mem->swap_pb = gtk_progress_bar_new();
        gtk_box_pack_start(GTK_BOX(mem->box), mem->swap_pb, FALSE, FALSE, 0);
        gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(mem->swap_pb), o);
        gtk_widget_set_size_request(mem->swap_pb, w, h);
    }

    gtk_widget_show_all(mem->box);
    gtk_container_add(GTK_CONTAINER(p->pwid), mem->box);
    gtk_widget_set_tooltip_markup(mem->plugin.pwid, "XXX");
    mem_update(mem);
    mem->timer = g_timeout_add(3000, (GSourceFunc) mem_update, (gpointer)mem);
    RET(1);
    
error:
    mem_destructor(p);
    RET(0);
}

static plugin_class class = {
    .fname       = NULL,
    .count       = 0,
    .type        = "mem",
    .name        = "Memory Monitor",
    .version     = "1.0",
    .description = "Show memory usage",
    .priv_size   = sizeof(mem_priv),

    .constructor = mem_constructor,
    .destructor  = mem_destructor,
};
static plugin_class *class_ptr = (plugin_class *) &class;

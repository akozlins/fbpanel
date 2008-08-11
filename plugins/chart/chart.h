#ifndef CHART_H
#define CHART_H


#include "plugin.h"
#include "panel.h"


/* chart.h */
typedef struct {
    plugin_priv plugin;
    GdkGC **gc_cpu;
    GtkWidget *da;

    gint **ticks;
    gint pos;
    gint w, h, rows;
} chart_priv;

typedef struct {
    int (*constructor)(plugin_priv *p);
    void (*destructor)(plugin_priv *p);
    void (*add_tick)(chart_priv *c, float *val);
    void (*set_rows)(chart_priv *c, int num, gchar *colors[]);
} chart_class_t;


#endif

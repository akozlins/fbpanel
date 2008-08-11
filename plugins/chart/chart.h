#ifndef CHART_H
#define CHART_H


#include "plugin.h"
#include "panel.h"


/* chart.h */
typedef struct {
    GdkGC **gc_cpu;
    GtkWidget *da;

    gint **ticks;
    gint pos;
    gint w, h, rows;
} chart_t;

typedef struct {
    int (*constructor)(plugin_priv *p);
    void (*destructor)(plugin_priv *p);
    void (*add_tick)(chart_t *c, float *val);
    void (*set_rows)(chart_t *c, int num, gchar *colors[]);
} chart_class_t;


#endif

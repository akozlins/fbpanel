#ifndef CHART_H
#define CHART_H


#include "plugin.h"
#include "panel.h"


/* chart.h */
typedef struct {
    GdkGC *gc_cpu;
    GdkColor *ccpu;
    GtkWidget *da;
    GtkTooltips *tip;

    gint *ticks;
    gint pos;
    gint w, h;
} chart_t;

typedef struct {
    int (*constructor)(plugin *p);
    void (*destructor)(plugin *p);
    void (*add_tick)(chart_t *c, float val);
} chart_class_t;


#endif

#ifndef meter_H
#define meter_H

#include "plugin.h"
#include "panel.h"

typedef struct {
    plugin_instance plugin;
    gchar **icons;
    gint num;
    GtkWidget *meter;
    gfloat level;
    gint cur_icon;
    gint size;
} meter_priv;

typedef struct {
    plugin_class plugin;
    void (*set_level)(meter_priv *c, gfloat *val);
    void (*set_icons)(meter_priv *c, gchar *th_icon, int num, gchar **mt_icons);
} meter_class;


#endif

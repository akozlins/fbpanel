
#ifndef PLUGIN_H
#define PLUGIN_H
#include <gmodule.h>


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include "panel.h"

struct _plugin_instance *stam;

typedef struct {
    /* common */
    char *fname;
    int count;
    GModule *gmodule;

    int dynamic : 1;
    int invisible : 1;
    /* these fields are pointers to the data within loaded dll */
    char *type;
    char *name;
    char *version;
    char *description;
    int priv_size;
    
    int (*constructor)(struct _plugin_instance *this);
    void (*destructor)(struct _plugin_instance *this);
    void (*save_config)(struct _plugin_instance *this, FILE *fp);
    GtkWidget *(*edit_config)(struct _plugin_instance *this);
} plugin_class;

#define PLUGIN_CLASS(class) ((plugin_class *) class)    

typedef struct _plugin_instance{
    plugin_class *class;
    panel        *panel;
    FILE         *fp;
    GtkWidget    *pwid;
    int           expand;
    int           padding;
    int           border;
    gpointer      priv;
} plugin_instance;

/* if plugin_instance is external it will load its dll */
plugin_instance * plugin_load(char *type);
void plugin_put(plugin_instance *this);
int plugin_start(plugin_instance *this);
void plugin_stop(plugin_instance *this);
GtkWidget *default_plugin_instance_edit_config(plugin_instance *pl);


#endif

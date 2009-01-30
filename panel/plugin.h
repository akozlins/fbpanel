
#ifndef PLUGIN_H
#define PLUGIN_H
#include <gmodule.h>


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include "panel.h"

struct _plugin_priv *stam;

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
    
    int (*constructor)(struct _plugin_priv *this);
    void (*destructor)(struct _plugin_priv *this);
    void (*save_config)(struct _plugin_priv *this, FILE *fp);
    GtkWidget *(*edit_config)(struct _plugin_priv *this);
} plugin_class;

#define PLUGIN_CLASS(class) ((plugin_class *) class)    

typedef struct _plugin_priv{
    plugin_class *class;
    panel        *panel;
    FILE         *fp;
    GtkWidget    *pwid;
    int           expand;
    int           padding;
    int           border;
    gpointer      priv;
} plugin_priv;

/* if plugin_priv is external it will load its dll */
plugin_priv * plugin_load(char *type);
void plugin_put(plugin_priv *this);
int plugin_start(plugin_priv *this);
void plugin_stop(plugin_priv *this);
GtkWidget *default_plugin_priv_edit_config(plugin_priv *pl);


#endif

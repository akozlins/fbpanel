/*
 * OSS battery plugin. Will works with ALSA since it usually 
 * emulates OSS layer.
 */

#include "misc.h"
#include "../meter/meter.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined __linux__
#include <linux/soundcard.h>
#endif

//#define DEBUGPRN
#include "dbg.h"

static gchar *names[] = {
    "gpm-battery-000",
    "gpm-battery-020",
    "gpm-battery-040",
    "gpm-battery-060",
    "gpm-battery-080",
    "gpm-battery-100",
};

static gchar *ch_names[] = {
    "gpm-battery-000-charging",
    "gpm-battery-020-charging",
    "gpm-battery-040-charging",
    "gpm-battery-060-charging",
    "gpm-battery-080-charging",
    "gpm-battery-100-charging",
};
  
typedef struct {
    meter_priv meter;
    int timer;
    int level;
    gboolean charging;
} battery_priv;

static meter_class *k;

static void
update_battery_status(battery_priv *c, int *level, gboolean *charging)
{
    *level = c->level + 2;
    *charging = c->charging;
    if (*level > 100) {
        *level = 0;
        *charging = !c->charging;
    }
}

static gboolean
battery_get_load(battery_priv *c)
{
    int level;
    gboolean charging;
    gchar buf[50];
    
    update_battery_status(c, &level, &charging);
    if (charging != c->charging) {
        if (charging)
            k->set_icons(&c->meter, G_N_ELEMENTS(ch_names), ch_names);
        else
            k->set_icons(&c->meter, G_N_ELEMENTS(names), names);
        DBG("seting %s icons\n", charging ? "charging" : "normal");
        c->charging = charging;
    }
    c->level = level;
    k->set_level(&c->meter, (gfloat) (level / 100.0));
    g_snprintf(buf, sizeof(buf), "<b>Battery:</b> %d%%%s",
        level, charging ? "\nCharging" : "");
    gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, buf);
    RET(TRUE);
}


#if 0

#endif

static int
battery_constructor(plugin_instance *p)
{
    battery_priv *c;
    
    if (!(k = class_get("meter")))
        RET(0);
    if (!PLUGIN_CLASS(k)->constructor(p))
        RET(0);
    c = (battery_priv *) p;
    k->set_icons(&c->meter, G_N_ELEMENTS(names), names);
    c->timer = g_timeout_add(1000, (GSourceFunc) battery_get_load,
        (gpointer) c);
    battery_get_load(c);
    RET(1);
}

static void
battery_destructor(plugin_instance *p)
{
    battery_priv *c = (battery_priv *) p;

    ENTER;
    g_source_remove(c->timer);
    PLUGIN_CLASS(k)->destructor(p);
    class_put("meter");
    RET();
}

static plugin_class class = {
    .count       = 0,
    .type        = "battery",
    .name        = "battery usage",
    .version     = "1.0",
    .description = "Display battery usage",
    .priv_size   = sizeof(battery_priv),
    .constructor = battery_constructor,
    .destructor  = battery_destructor,
};

static plugin_class *class_ptr = (plugin_class *) &class;

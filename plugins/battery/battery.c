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

typedef struct {
    meter_priv meter;
    int timer;
    int level;
    gboolean charging;
    gchar *sys;
} battery_priv;

static meter_class *k;

static gchar *batt_working[] = {
    "battery_0",
    "battery_1",
    "battery_2",
    "battery_3",
    "battery_4",
    "battery_5",
    "battery_6",
    "battery_7",
    "battery_8",  
};
static gchar **batt_charging = batt_working;
static gchar *batt_na[] = {
    "battery_na",
};

/* Searches in /sys/class/power_supply/ for first directory
 * representing active battery.
 * Returns
 *   Pointer to allocated directory path, must be freed by caller
 *   NULL - if no battery was found
 */
static gchar *
battery_find_sys(void)
{
    return NULL;
}

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
            k->set_icons(&c->meter, G_N_ELEMENTS(batt_charging), batt_charging);
        else
            k->set_icons(&c->meter, G_N_ELEMENTS(batt_working), batt_working);
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


static int
battery_constructor(plugin_instance *p)
{
    battery_priv *c;
    
    if (!(k = class_get("meter")))
        RET(0);
    if (!PLUGIN_CLASS(k)->constructor(p))
        RET(0);
    c = (battery_priv *) p;
    c->sys = battery_find_sys();
    if (c->sys) {
        k->set_icons(&c->meter, G_N_ELEMENTS(batt_working), batt_working);
        c->timer = g_timeout_add(1000, (GSourceFunc) battery_get_load,
            (gpointer) c);
        battery_get_load(c);
    } else {
        k->set_icons(&c->meter, G_N_ELEMENTS(batt_na), batt_na);
        k->set_level(&c->meter, 0.0);
        gtk_widget_set_tooltip_markup(p->pwid,
            "Runing on AC\nNo battery found");
    }
    RET(1);
}

static void
battery_destructor(plugin_instance *p)
{
    battery_priv *c = (battery_priv *) p;

    ENTER;
    if (c->timer)
        g_source_remove(c->timer);
    g_free(c->sys);
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

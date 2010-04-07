#include "misc.h"
#include "../meter/meter.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define DEBUGPRN
#include "dbg.h"

typedef struct {
    meter_priv meter;
    int timer;
    gfloat level;
    gboolean charging;
    gboolean exist;
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
    NULL
};

static gchar **batt_charging = batt_working;
static gchar *batt_na[] = {
    "battery_na",
    NULL
};

#if defined __linux__

static void
battery_update_os(battery_priv *c)
{
    c->exist = TRUE;
    c->level += 2;
    if (c->level > 100) {
        c->level = 0;
        c->charging = !c->charging;
    }
}

#else

static void
battery_update_os(battery_priv *c)
{
    c->exist = FALSE;
}

#endif

static gboolean
battery_update(battery_priv *c)
{
    gchar buf[50];
    gchar **i;
    
    battery_update_os(c);
    if (c->exist) {
        i = c->charging ? batt_charging : batt_working;
        g_snprintf(buf, sizeof(buf), "<b>Battery:</b> %d%%%s",
            (int) c->level, c->charging ? "\nCharging" : "");
        gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, buf);
    } else {
        i = batt_na;
        gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid,
            "Runing on AC\nNo battery found");
    }
    if (i != ((meter_priv *) c)->icons)
        k->set_icons(&c->meter, i);
    k->set_level(&c->meter, c->level);
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
    c->timer = g_timeout_add(1000, (GSourceFunc) battery_update,
        (gpointer) c);
    battery_update(c);
    RET(1);
}

static void
battery_destructor(plugin_instance *p)
{
    battery_priv *c = (battery_priv *) p;

    ENTER;
    if (c->timer)
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

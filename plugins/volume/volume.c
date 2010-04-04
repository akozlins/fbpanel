/*
 * OSS volume plugin. Will works with ALSA since it usually 
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
    "stock_volume-min",
    "stock_volume-med",
    "stock_volume-max",
};

static gchar *s_names[] = {
    "stock_volume-mute",
};
  
typedef struct {
    meter_priv meter;
    int fd, chan;
    guchar vol, muted_vol;
    int timer;
    gboolean muted;
    GtkWidget *pbar_window;
} volume_priv;

static meter_class *k;


static gboolean
volume_get_load(volume_priv *c)
{
    int volume;
    gchar buf[20];

    ENTER;
    if (ioctl(c->fd, MIXER_READ(c->chan), &volume)) {
        ERR("volume: can't get volume from /dev/mixer\n");
        RET(FALSE);
    }
    DBG("chan=%d vol=%d/%d oldvol=%d\n", c->chan, volume & 0xFF,
        (volume >> 8) & 0xFF, c->vol);
    volume &= 0xFF;
    if ((volume != 0) != (c->vol != 0)) {
        if (volume)
            k->set_icons(&c->meter, G_N_ELEMENTS(names), names);
        else
            k->set_icons(&c->meter, G_N_ELEMENTS(s_names), s_names);
        DBG("seting %s icons\n", volume ? "normal" : "muted");
    }
    c->vol = volume;
    k->set_level(&c->meter, (gfloat) (volume / 100.0));
    g_snprintf(buf, sizeof(buf), "<b>Volume:</b> %d%%", volume);
    gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, buf);
    RET(TRUE);
}

static GtkWidget *
volume_create_pbar(volume_priv *c)
{
    GtkWidget *pbar, *win;
    int o[] = {
        [EDGE_BOTTOM] = GTK_PROGRESS_BOTTOM_TO_TOP,
        [EDGE_TOP] = GTK_PROGRESS_TOP_TO_BOTTOM,
        [EDGE_LEFT] = GTK_PROGRESS_LEFT_TO_RIGHT,
        [EDGE_RIGHT] = GTK_PROGRESS_RIGHT_TO_LEFT,
    };

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(win), 180, 180);
    gtk_window_set_decorated(GTK_WINDOW(win), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(win), 5);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(win), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(win), TRUE);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_MOUSE);
    gtk_window_stick(GTK_WINDOW(win));
          
    pbar = gtk_progress_bar_new();
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(pbar),
        o[((plugin_instance *)c)->panel->edge]);
    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(pbar));
 
    return win;
}  


static gboolean
clicked(GtkWidget *widget, GdkEventButton *event, volume_priv *c)
{
    int volume;

    if (event->button == 1 && event->type == GDK_BUTTON_PRESS) {
        if (c->pbar_window == NULL)
        {
            c->pbar_window = volume_create_pbar(c);
            gtk_widget_show_all(c->pbar_window);
            gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, NULL);
        }
        else
        {
            gtk_widget_destroy(c->pbar_window);
            c->pbar_window = NULL;
        }
    }
    if (!(event->type == GDK_BUTTON_PRESS && event->button == 2))
        RET(FALSE);
    
    if (c->muted) {
        volume = (c->muted_vol << 8) | c->muted_vol;
    } else {
        c->muted_vol = c->vol;
        volume = 0;
    }
    ioctl(c->fd, MIXER_WRITE(c->chan), &volume);
    c->muted = !c->muted;
    DBG("btn 2 press - %smute\n", c->muted ? "" : "un");
    volume_get_load(c);
    RET(FALSE);
}

static gboolean
scrolled(GtkWidget *widget, GdkEventScroll *event, volume_priv *c)
{
    gchar i;
    int volume;
    
    ENTER;

    if (c->muted) {
        i = c->muted_vol;
    } else {
        ioctl(c->fd, MIXER_READ(c->chan), &volume);
        i = volume & 0xFF;
    }
    i += 2 * ((event->direction == GDK_SCROLL_UP
            || event->direction == GDK_SCROLL_LEFT) ? 1 : -1);
    if (i > 100)
        i = 100;
    if (i < 0)
        i = 0;
    
    if (c->muted) {
        c->muted_vol = i;
    } else {
        volume = (i << 8) | i;
        ioctl(c->fd, MIXER_WRITE(c->chan), &volume);
    }
    DBG("seting vol=%d\n", i);
    volume_get_load(c);
    return FALSE;
}



#if 0
static gchar *names[] = {
    "gpm-battery-000-charging",
    "gpm-battery-020-charging",
    "gpm-battery-040-charging",
    "gpm-battery-060-charging",
    "gpm-battery-080-charging",
    "gpm-battery-100-charging",
};

#endif


    
static int
volume_constructor(plugin_instance *p)
{
    volume_priv *c;
    
    if (!(k = class_get("meter")))
        RET(0);
    if (!PLUGIN_CLASS(k)->constructor(p))
        RET(0);
    c = (volume_priv *) p;
    if ((c->fd = open ("/dev/mixer", O_RDWR, 0)) < 0) {
        ERR("volume: can't open /dev/mixer\n");
        RET(0);
    }
    k->set_icons(&c->meter, G_N_ELEMENTS(names), names);
    c->timer = g_timeout_add(1000, (GSourceFunc) volume_get_load, (gpointer) c);
    c->vol = 200;
    c->chan = SOUND_MIXER_VOLUME;
    volume_get_load(c);
    g_signal_connect(G_OBJECT(p->pwid), "scroll-event",
        G_CALLBACK(scrolled), (gpointer) c);
    g_signal_connect(G_OBJECT(p->pwid), "button_press_event",
        G_CALLBACK(clicked), (gpointer)c);

    RET(1);
}


static void
volume_destructor(plugin_instance *p)
{
    volume_priv *c = (volume_priv *) p;

    ENTER;
    g_source_remove(c->timer);
    PLUGIN_CLASS(k)->destructor(p);
    class_put("meter");
    RET();
}



static plugin_class class = {
    .count       = 0,
    .type        = "volume",
    .name        = "Volume",
    .version     = "1.0",
    .description = "OSS volume control",
    .priv_size   = sizeof(volume_priv),
    .constructor = volume_constructor,
    .destructor  = volume_destructor,
};

static plugin_class *class_ptr = (plugin_class *) &class;

/*
 * CPU usage plugin to fbpanel
 *
 * Copyright (C) 2004 by Alexandre Pereira da Silva <alexandre.pereira@poli.usp.br>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 */
/*A little bug fixed by Mykola <mykola@2ka.mipt.ru>:) */


#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

#include "plugin.h"
#include "panel.h"
#include "gtkbgbox.h"
#include "chart.h"


#define KILOBYTE 1024
#define MAX_WGSIZE 100

//#define DEBUG
#include "dbg.h"



/* chart.c */
static void
chart_add_tick(chart_t *c, float val)
{
    ENTER;
    if (!c->ticks)
        RET();

    if (val < 0)
        val = 0;
    if (val > 1)
        val = 1;
    c->ticks[c->pos] = val * c->h;
    DBG("new wval = %uld\n", c->ticks[c->pos]);
    c->pos = (c->pos + 1) %  c->w;
    gtk_widget_queue_draw(c->da);

    RET();
}

static void
chart_draw(chart_t *c)
{
    int i;

    ENTER;
    DBG("c->w=%d\n", c->w);
    DBG("c->h=%d\n", c->h);
    if (!c->ticks)
        RET();
    for (i = 1; i < c->w-1; i++) {
    	int val;
	
	    val = c->ticks[(i + c->pos) % c->w];
        if (val)
            gdk_draw_line(c->da->window, c->gc_cpu, i, c->h-2, i, c->h - val + 1);
    }
    RET();
}

static void
chart_size_allocate(GtkWidget *widget, GtkAllocation *a, chart_t *c)
{
    ENTER;
    if (c->ticks)
        g_free(c->ticks);
    c->ticks = NULL;
    c->w = widget->allocation.width;
    c->h = widget->allocation.height;
    if (c->w < 2 || c->h < 2) 
        RET();
    DBG("c->w=%d\n", c->w);
    DBG("c->h=%d\n", c->h);
    c->ticks = g_new0( typeof(*c->ticks), c->w);
    c->pos = 0;
    
   RET();
}


static gint
chart_expose_event(GtkWidget *widget, GdkEventExpose *event, chart_t *c)
{
    ENTER;
    gdk_window_clear(widget->window);
    chart_draw(c);
    gdk_draw_rectangle(widget->window, 
            //widget->style->black_gc,                       
            widget->style->bg_gc[GTK_STATE_NORMAL],
            FALSE, 0, 0, c->w-1, c->h-1);
    RET(FALSE);
}



static int
chart_constructor(plugin *p)
{
    chart_t *c;

    ENTER;
    /* must be allocated by caller */
    c = (chart_t *)  p->priv;
 
    c->da = gtk_drawing_area_new();
    c->da = p->pwid;
    gtk_widget_set_size_request(c->da, 40, 20);


    c->gc_cpu = gdk_gc_new(p->panel->topgwin->window);
    c->ccpu = (GdkColor *)malloc(sizeof(GdkColor));
    gdk_color_parse("green",  c->ccpu);
    gdk_colormap_alloc_color(gdk_drawable_get_colormap(p->panel->topgwin->window),  c->ccpu, FALSE, TRUE);
    gdk_gc_set_foreground(c->gc_cpu,  c->ccpu);
    gtk_container_set_border_width (GTK_CONTAINER (p->pwid), 1);
    g_signal_connect (G_OBJECT (p->pwid), "size-allocate",
            G_CALLBACK (chart_size_allocate), (gpointer) c);

    g_signal_connect_after (G_OBJECT (p->pwid), "expose-event",
          G_CALLBACK (chart_expose_event), (gpointer) c);
    
    RET(1);
}

static void
chart_destructor(plugin *p)
{
    chart_t *c = (chart_t *) p->priv;

    ENTER;
    g_object_unref(c->gc_cpu);
    g_free(c->ticks);
    g_free(c->ccpu);
    RET();
}


chart_class_t class = {
    constructor : chart_constructor,
    destructor  : chart_destructor,
    add_tick    : chart_add_tick,
};

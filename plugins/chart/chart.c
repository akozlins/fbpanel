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


//#define DEBUG
#include "dbg.h"


static void chart_free_rows(chart_t *c);
static void chart_add_tick(chart_t *c, float *val);
static void chart_draw(chart_t *c);
static void chart_size_allocate(GtkWidget *widget, GtkAllocation *a, chart_t *c);
static gint chart_expose_event(GtkWidget *widget, GdkEventExpose *event, chart_t *c);

extern panel *the_panel;



static void
chart_add_tick(chart_t *c, float *val)
{
    int i;

    ENTER;
    if (!c->ticks)
        RET();
    for (i = 0; i < c->rows; i++) {
        if (val[i] < 0)
            val[i] = 0;
        if (val[i] > 1)        
            val[i] = 1;
        c->ticks[i][c->pos] = val[i] * c->h;
        DBG("new wval = %uld\n", c->ticks[i][c->pos]);
    }
    c->pos = (c->pos + 1) %  c->w;
    gtk_widget_queue_draw(c->da);

    RET();
}

static void
chart_draw(chart_t *c)
{
    int j, i, y;

    ENTER;
    DBG("c->w=%d\n", c->w);
    DBG("c->h=%d\n", c->h);
    if (!c->ticks)
        RET();
    for (i = 1; i < c->w-1; i++) {
        y = c->h-2;
        for (j = 0; j < c->rows; j++) {
            int val;
	
            val = c->ticks[j][(i + c->pos) % c->w];
            if (val)
                gdk_draw_line(c->da->window, c->gc_cpu[j], i, y, i, y - val);
            y -= val;
        }
    }
    RET();
}

static void
chart_size_allocate(GtkWidget *widget, GtkAllocation *a, chart_t *c)
{
    int i;

    ENTER;
    c->w = widget->allocation.width;
    c->h = widget->allocation.height;
    if (c->w < 2 || c->h < 2) 
        RET();
    for (i = 0; i < c->rows; i++) {
        g_free(c->ticks[i]);
        c->ticks[i] = g_new0(typeof(**c->ticks), c->w);
    }
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



static void
chart_free_rows(chart_t *c)
{
    int i;

    ENTER;
    for (i = 0; i < c->rows; i++) {
        g_free(c->ticks[i]);
        g_object_unref(c->gc_cpu[i]);
    }
    g_free(c->ticks);
    g_free(c->gc_cpu);
    c->rows = 0;
    c->ticks = NULL;
    c->gc_cpu = NULL;

    RET();
}

static void
chart_set_rows(chart_t *c, int num, gchar *colors[])
{
    ENTER;
    g_assert(num > 0 && num < 10);
    chart_free_rows(c);
    c->rows = num;
    c->pos = 0;
    c->ticks = g_new0( typeof(*c->ticks), c->rows);
    c->gc_cpu = g_new0( typeof(*c->gc_cpu), c->rows);
    if (!c->ticks || !c->gc_cpu) {
        ERR("chart: can't allocate mem\n");
        RET();
    }
    while (num-- > 0) {
        GdkColor color;

        DBG("num=%d \n", num);
        DBG("color=%s\n", colors[num]);
        c->gc_cpu[num] = gdk_gc_new(the_panel->topgwin->window);
                
        gdk_color_parse(colors[num], &color);
        gdk_colormap_alloc_color(gdk_drawable_get_colormap(the_panel->topgwin->window),  &color, FALSE, TRUE);
        gdk_gc_set_foreground(c->gc_cpu[num],  &color);
        c->ticks[num] = g_new0(typeof(**c->ticks), c->w);

    }
    RET();
}

static int
chart_constructor(plugin *p)
{
    chart_t *c;

    ENTER;
    /* must be allocated by caller */
    c = (chart_t *)  p->priv;
 
    c->da = p->pwid;
    gtk_widget_set_size_request(c->da, 40, 20);
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
    chart_free_rows(c);
    RET();
}

chart_class_t class = {
    .constructor = chart_constructor,
    .destructor  = chart_destructor,
    .add_tick    = chart_add_tick,
    .set_rows    = chart_set_rows,
};

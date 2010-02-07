#include <stdlib.h>
#include <string.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <glib/gstdio.h>
full_
#include "panel.h"
#include "misc.h"
#include "plugin.h"
#include "bg.h"
#include "gtkbgbox.h"

//#define DEBUGPRN
#include "dbg.h"
static void
spawn_app(GtkWidget *widget, gpointer data)
{
    GError *error = NULL;

    ENTER;
    if (data) {
        if (! g_spawn_command_line_async(data, &error) ) {
            ERR("can't spawn %s\nError is %s\n", (char *)data, error->message);
            g_error_free (error);
        }
    }
    RET();
}

static void
menu_item_set_image(GtkWidget *mi, gchar *iname, gchar *fname, int width, int height)
{
    GdkPixbuf *pb;

    ENTER;
    if ((pb = fb_pixbuf_new(iname, fname, 22, 22, FALSE))) {
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi),
                gtk_image_new_from_pixbuf(pb));
        g_object_unref(G_OBJECT(pb));
    }
    RET();
}


static void
run_command(GtkWidget *widget, void (*cmd)(void))
{
    ENTER;
    cmd();
    RET();
}

static gboolean
my_button_pressed(GtkWidget *widget, GdkEventButton *event, plugin_instance *p)
{
    menu_priv *m;

    ENTER;
    if (event->type == GDK_BUTTON_PRESS && event->button == 3
          && event->state & GDK_CONTROL_MASK) {
        RET(FALSE);
    }
    m = (menu_priv *) p;
    if ((event->type == GDK_BUTTON_PRESS)
          && (event->x >=0 && event->x < widget->allocation.width)
          && (event->y >=0 && event->y < widget->allocation.height)) {
        if (!m->menu) {
            DBG("building menu\n");
            m->xc = menu_expand_xc(p->xc);
            read_submenu(p, TRUE);
            gtk_widget_show_all(m->menu);
        }
        gtk_menu_popup(GTK_MENU(m->menu),
              NULL, NULL, (GtkMenuPositionFunc)menu_pos, widget,
              event->button, event->time);
    }
    RET(TRUE);
}


static GtkWidget *
make_button(plugin_instance *p, gchar *iname, gchar *fname, gchar *name)
{
    int w, h;
    menu_priv *m;

    ENTER;
    m = (menu_priv *) p;
    /* XXX: this code is duplicated in every plugin.
     * Lets run it once in a panel */
    if (p->panel->orientation == ORIENT_HORIZ) {
        w = -1;
        h = p->panel->ah;
    } else {
        w = p->panel->aw;
        h = -1;
    }
    DBG("iname=%s\n", iname);
    m->bg = fb_button_new(iname, fname, w, h, 0x702020, name);
    gtk_widget_show(m->bg);
    gtk_box_pack_start(GTK_BOX(m->box), m->bg, FALSE, FALSE, 0);
    if (p->panel->transparent)
        gtk_bgbox_set_background(m->bg, BG_INHERIT, p->panel->tintcolor,
            p->panel->alpha);

    m->handler_id = g_signal_connect (G_OBJECT (m->bg), "button-press-event",
          G_CALLBACK (my_button_pressed), p);
    g_object_set_data(G_OBJECT(m->bg), "plugin", p);
    gtk_widget_show_all(p->pwid);
    RET(m->bg);
}

static GtkWidget *
read_item(plugin_instance *p)
{
    line s;
    gchar *name, *fname, *iname, *action;
    GtkWidget *item;
    void (*cmd)(void);

    ENTER;
    name = fname = action = iname = NULL;
    cmd = NULL;
    XCG(xc, "image", &fname, str);
    XCG(xc, "icon", &iname, str);
    XCG(xc, "name", &name, str);
    XCG(xc, "action", &action, str);
    XCG(xc, "command", &command, str);
    fname = expand_tilda(fname);
    action = expand_tilda(action);
    for (tmp = commands; tmp->name; tmp++)
        if (!g_ascii_strcasecmp(s.t[1], tmp->name))
        {
            cmd = tmp->cmd;
            break;
        }

    /* menu button */
    item = gtk_image_menu_item_new_with_label(name ? name : "");
    gtk_container_set_border_width(GTK_CONTAINER(item), 0);
    if (fname || iname) 
        menu_item_set_image(item, iname, fname, 22, 22);
    g_free(fname);
    if (cmd) {
        g_signal_connect(G_OBJECT(item), "activate",
            (GCallback)run_command, cmd);
    }
    else if (action)
    {
        g_signal_connect(G_OBJECT(item), "activate",
            (GCallback)spawn_app, action);
        g_object_set_data_full(G_OBJECT(item), "activate", action, g_free);
    }
    RET(item);
}

static GtkWidget *
read_separator()
{
    ENTER;
    RET(gtk_separator_menu_item_new());
}


static void
read_system_menu(plugin_instance *p, GtkWidget *menu)
{
    ENTER;
    make_fdo_menu(p, menu);
    RET();
}

static void
read_include()
{
    ENTER;
    /* XXX: implement global #include directive */
    ERR("Include block in menu is obsolete.\n"
        "Use #include directive instead\n.");
    RET();
}

static GtkWidget *
read_submenu(plugin_instance *p, gboolean as_item)
{
    line s;
    GtkWidget *mi, *menu;
    gchar name[256], *fname, *iname;
    menu_priv *m = (menu_priv *)p;


    ENTER;
    menu = gtk_menu_new ();
    gtk_container_set_border_width(GTK_CONTAINER(menu), 0);

    iname = fname = NULL;
    name[0] = 0;
    DBG("here\n");
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_BLOCK_START) {
            if (!as_item)
                break;
            mi = NULL;
            if (!g_ascii_strcasecmp(s.t[0], "item")) {
                mi = read_item(p);
            } else if (!g_ascii_strcasecmp(s.t[0], "separator")) {
                mi = read_separator(p);
            } else if (!g_ascii_strcasecmp(s.t[0], "menu")) {
                mi = read_submenu(p, TRUE);
            } else if (!g_ascii_strcasecmp(s.t[0], "systemmenu")) {
                read_system_menu(p, menu);
                continue;
            } else if (!g_ascii_strcasecmp(s.t[0], "include")) {
                read_include(p);
                continue;
            } else {
                ERR("menu: unknown block %s\n", s.t[0]);
                goto error;
            }
            if (!mi) {
                ERR("menu: can't create menu item\n");
                goto error;
            }
            gtk_widget_show(mi);
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
        } else if (s.type == LINE_VAR) {
            DBG("here\n");
            if (!g_ascii_strcasecmp(s.t[0], "image"))
                fname = expand_tilda(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "name"))
                strcpy(name, s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "icon")) {
                iname = g_strdup(s.t[1]);
                DBG("icon\n");
            } else {
                ERR("menu: unknown var %s\n", s.t[0]);
                goto error;
            }
            DBG("here\n");
        } else if (s.type == LINE_NONE) {
            if (m->files) {
                fclose(p->fp);
                p->fp = m->files->data;
                m->files = g_slist_delete_link(m->files, m->files);
            }
        }  else {
            ERR("menu: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    DBG("here\n");
    if (as_item) {
        mi = gtk_image_menu_item_new_with_label(name);
        if (fname || iname) 
            menu_item_set_image(mi, iname, fname, 22, 22);
        gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), menu);
        m->menu = menu;
    } else {
        mi = make_button(p, iname, fname, name);
    }
    g_free(fname);
    g_free(iname);
    RET(mi);

 error:
    // FIXME: we need to recursivly destroy all child menus and their items
    DBG("destroy(menu)\n");
    gtk_widget_destroy(menu);
    g_free(fname);
    g_free(name);
    RET(NULL);
}

static GtkWidget *
menu_create_separator()
{
    return gtk_separator_menu_item_new();
}

/* Ccreates menu item. Text and image are read from xconf. Action
 * depends on @menu. If @menu is NULL, action is to execute external
 * command. Otherwise it is to pop up @menu menu */
static GtkWidget *
menu_create_item(xconf *xc, GtkWidget *menu)
{
   name = fname = action = iname = NULL;
   cmd = NULL;
  
   XCG(xc, "name", &name, str);
   mi = gtk_image_menu_item_new_with_label(name ? name : "");
   gtk_container_set_border_width(GTK_CONTAINER(item), 0);
   XCG(xc, "image", &fname, str);
   fname = expand_tilda(fname);
   XCG(xc, "icon", &iname, str);
   if (fname || iname) 
       menu_item_set_image(item, iname, fname, 22, 22);
   g_free(fname);

   if (menu)
   {
       gtk_menu_item_set_submenu(GTK_MENU_ITEM(mi), menu);
       goto done;
   }
   XCG(xc, "action", &action, str);
   if (action)
   {
       action = expand_tilda(action);
       g_signal_connect(G_OBJECT(mi), "activate",
           (GCallback)spawn_app, action);
       g_object_set_data_full(G_OBJECT(mi), "activate",
           action, g_free);
       goto done;
   }
   XCG(xc, "command", &command, str);
   if (command)
   {
       for (tmp = commands; tmp->name; tmp++)
           if (!g_ascii_strcasecmp(command, tmp->name))
           {
               g_signal_connect(G_OBJECT(mi), "activate",
                   (GCallback)run_command, tmp->cmd);
               goto done;
           }
   }

done:
   return mi;
}

/* Creates menu and optionally button to pop it up.
 * If @ret_menu is TRUE, then a menu is returned. Otherwise,
 * button is created, linked to a menu and returned instead. */
static GtkWidget *
menu_create_menu(xconf *xc, gboolean ret_menu)
{
    GtkWidget *mi, *menu;

    if (!xc)
        return NULL;
    menu = gtk_menu_new ();
    gtk_container_set_border_width(GTK_CONTAINER(menu), 0);
    for (w = xc->sons; w ; w = g_slist_next(w))
    {
        nxc = w->data;
        if (!strcmp(nxc->name, "separator"))
            mi = menu_create_separator();
        else if (!strcmp(nxc->name, "item"))
            mi = menu_create_item(nxc, NULL);
        else if (!strcmp(nxc->name, "menu"))
            mi = menu_create_menu(nxc, FALSE);
        else
            continue;
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);
    }
    gtk_widget_show_all(menu);
    return (ret_menu) ? menu : menu_create_item(xc, menu);
}

static void
menu_create(plugin_instance *p)
{
    menu_priv *m = (menu_priv *) p;

    m->xc = menu_expand_xc(p->xc, NULL);
    m->menu = menu_create_menu(m->xc, TRUE);
}

static void
menu_destroy(GtkIconTheme *icon_theme, plugin_instance *p)
{
    menu_priv *m;

    ENTER;
    m = (menu_priv *) p;
    if (m->menu)
    {
        DBG("destroy(m->menu)\n");
        gtk_widget_destroy(m->menu);
        m->menu = NULL;
    }
    if (m->xc)
    {
        xconf_del(m->xc);
        m->xc = NULL;
    }
}


static int
menu_constructor(plugin_instance *p)
{
    menu_priv *m;

    ENTER;
    m = (menu_priv *) p;
    m->iconsize = 22;
    m->box = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(m->box), 0);
    gtk_container_add(GTK_CONTAINER(p->pwid), m->box);

    menu_init(p);
    g_signal_connect (G_OBJECT(gtk_icon_theme_get_default()),
        "changed", (GCallback) menu_destroy, p);
    RET(1);
}


static void
menu_destructor(plugin_instance *p)
{
    menu_priv *m = (menu_priv *) p;

    ENTER;
    if (m->tout)
        g_source_remove(m->tout);
    g_signal_handler_disconnect(G_OBJECT(m->bg), m->handler_id);
    g_signal_handlers_disconnect_by_func(G_OBJECT(gtk_icon_theme_get_default()),
        destroy_menu, p);
    if (m->menu) {
        DBG("destroy(m->menu)\n");
        gtk_widget_destroy(m->menu);
    }
    DBG("destroy(m->box)\n");
    gtk_widget_destroy(m->box);
    RET();
}


static plugin_class class = {
    .count       = 0,
    .type        = "menu",
    .name        = "Menu",
    .version     = "1.0",
    .description = "Menu",
    .priv_size   = sizeof(menu_priv),

    .constructor = menu_constructor,
    .destructor  = menu_destructor,
};

static plugin_class *class_ptr = (plugin_class *) &class;

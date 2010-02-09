
#include "gconf.h"

//#define DEBUGPRN
#include "dbg.h"

static GtkWidget *dialog;


static void
gconf_edit_int_cb(GtkSpinButton *w, xconf *xc)
{
    int i;
    ENTER;
    i = (int) gtk_spin_button_get_value(w);
    xconf_set_value(xc, itoa(i));
    RET();
}

GtkWidget *
gconf_edit_int(xconf *xc, int min, int max, (GCallback) cb, gpointer data)
{
    gint i;
    GtkWidget *w;
    
    xconf_get_int(xc, &i);
    w = gtk_spin_button_new_with_range((gdouble) min, (gdouble) max, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), (gdouble) i);
    g_signal_connect(G_OBJECT(w), "value-changed",
        G_CALLBACK(gconf_edit_int_cb), xc);
    return w;
}


static GtkWidget *
mk_tab_general(xconf *xc)
{
    GtkWidget *frame, *page;

    ENTER;
    page = gtk_vbox_new(FALSE, 1);
    frame = gconf_edit_int(xconf_find(xc, "width", 0), 0, 300, NULL, NULL);
    gtk_box_pack_start(GTK_BOX (page), frame, FALSE, TRUE, 0);
    /*
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW (sw), page);
    */
    RET(page);
}


static GtkWidget *
mk_dialog()
{
    GtkWidget *sw, *nb, *label;
    gchar *name;
    

    ENTER;
    DBG("creating dialog\n");
    //name = g_strdup_printf("fbpanel settings: <%s> profile", cprofile);
    name = g_strdup_printf("fbpanel settings: profile");
    dialog = gtk_dialog_new_with_buttons (name,
          NULL,
          GTK_DIALOG_NO_SEPARATOR, //GTK_DIALOG_DESTROY_WITH_PARENT,
          GTK_STOCK_CLOSE,
          GTK_RESPONSE_CLOSE,
          NULL);
    g_free(name);
    DBG("connecting sugnal to %p\n",  dialog);
#if 0    
    g_signal_connect (G_OBJECT(dialog), "response",     (GCallback) dialog_response_event, NULL);
    g_signal_connect (G_OBJECT(dialog), "destroy",      (GCallback) dialog_destroy_event, NULL);
    g_signal_connect (G_OBJECT(dialog), "delete_event", (GCallback) dialog_delete_event,  NULL);
#endif
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 500);
    gtk_window_set_icon_from_file(GTK_WINDOW(dialog), IMGPREFIX "/star.png", NULL);
      
    nb = gtk_notebook_new();
    gtk_notebook_set_show_border (GTK_NOTEBOOK(nb), FALSE);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), nb);

    sw = mk_tab_general();
    label = gtk_label_new("General");
    gtk_misc_set_padding(GTK_MISC(label), 4, 1);
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), sw, label);

    gtk_widget_show_all(dialog);
    RET(dialog);
}



void
configure(void)
{
    ENTER;
    DBG("dialog %p\n",  dialog);
    if (!dialog) 
        dialog = mk_dialog();
    gtk_widget_show(dialog);

    RET();
}

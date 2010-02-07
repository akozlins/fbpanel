






#if 0


/* Menu plugin creates menu from description found in config file or/and from
 * application files (aka system nenu).
 *
 * System menu note
 * Every directory is processed only once though it may be mentioned many times
 * in application directory list. This is to prevent duplicate entries.
 * For example, on my system '/usr/share' usually mentioned twise
 *    $ echo $XDG_DATA_DIRS
 *    /usr/share:/usr/share:/usr/local/share
 *
 * Icon Theme change note
 * When icon theme changes, entire menu is destroyed and scheduled for delayed
 * rebuild. Thus at any time menu uses small, fast and lightweight icons.
 * As opposite to approach where every icon wasts code and data to keep
 * track of icon them change and then reloads itself
 * FIXME: panel button that pops up the menu remains as is.
 *
 * Engine
 * The engine consists of 3 function and driver that calls them
 * make_button - creates a menu button with icon and text. Click on it will
 *   launch some application
 * make_separator - creates a visual separator between buttons
 * make_menu - creates a button with icon and text that will pop up another menu
 *   on a click
 */

static const char desktop_ent[] = "Desktop Entry";
static const gchar app_dir_name[] = "applications";
static GtkWidget *read_submenu(plugin_instance *p, gboolean as_item);

typedef struct {
    gchar *name;
    gchar *icon;
    gchar *local_name;
    GtkWidget *menu;
} cat_info;

static cat_info main_cats[] = {
    { "AudioVideo", "applications-multimedia", "Audio & Video" },
    { "Development","applications-development" },
    //{ "Education",  "applications-education" },
    { "Education",  "applications-other" },
    { "Game",       "applications-games" },
    { "Graphics",   "applications-graphics" },
    { "Network",    "applications-internet" },
    { "Office",     "applications-office" },
    { "Settings",   "preferences-system" },
    { "System",     "applications-system" },
    { "Utility",    "applications-utilities" },
};
static void destroy_menu(GtkIconTheme *icon_theme, plugin_instance *p);

/* Inserts menu item into menu sorted by name */
static gint
_menu_shell_insert_sorted(GtkMenuShell *menu_shell, GtkWidget *mi, const gchar *name)
{
    GList *items;
    gint i;
    gchar *cmpname;

    //TRACE("dummy");

    items = gtk_container_get_children(GTK_CONTAINER(menu_shell));
    for(i=0; items; items=items->next, i++)  {
        cmpname = (gchar *)g_object_get_data(G_OBJECT(items->data), "item-name");
        if(cmpname && g_ascii_strcasecmp(name, cmpname) < 0)
            break;
    }
    gtk_menu_shell_insert(menu_shell, mi, i);
    return i;
}

/* Scans directory 'path' for application files and builds corresponding entries
 * in category menus. If application belongs to several categories, first one
 * is used.
 */
static void
do_app_dir(plugin_instance *p, const gchar *path)
{
    GDir* dir;
    const gchar* name;
    gchar *cwd, **cats, **tmp, *exec, *title, *icon, *dot;
    GKeyFile*  file;
    menu_priv *m = (menu_priv *) p;

    ENTER;
    DBG("path: %s\n", path);
    // skip already proceeded dirs to prevent duplicate entries
    if (g_hash_table_lookup(m->ht, path))
        RET();
    g_hash_table_insert(m->ht, (void *)path, p);
    dir = g_dir_open(path, 0, NULL);
    if (!dir)
        RET();
    cwd = g_get_current_dir();
    g_chdir(path);
    file = g_key_file_new();
    while ((name = g_dir_read_name(dir))) {
        DBG("name: %s\n", name);
        if (g_file_test(name, G_FILE_TEST_IS_DIR)) {
            do_app_dir(p, name);
            continue;
        }
        if (!g_str_has_suffix(name, ".desktop"))
            continue;
        if (!g_key_file_load_from_file(file, name, 0, NULL))
            continue;
        if (g_key_file_get_boolean(file, desktop_ent, "NoDisplay", NULL))
            continue;
        if (g_key_file_has_key(file, desktop_ent, "OnlyShowIn", NULL))
            continue;
        if (!(cats = g_key_file_get_string_list(file, desktop_ent, "Categories", NULL, NULL)))
            continue;
        if (!(exec = g_key_file_get_string(file, desktop_ent, "Exec", NULL)))
            goto free_cats;

        /* ignore program arguments */
        while ((dot = strchr(exec, '%'))) {
            if (dot[1] != '\0')
                dot[0] = dot[1] = ' ';
        }
        DBG("exec: %s\n", exec);
        if (!(title = g_key_file_get_locale_string(file, desktop_ent, "Name", NULL, NULL)))
            goto free_exec;
        DBG("title: %s\n", title);
        icon = g_key_file_get_string(file, desktop_ent, "Icon", NULL);
        if (icon) {
            /* if icon is not a absolute path, drop an extenstion (if any)
             * to allow to load it as themable icon */
            dot = strchr( icon, '.' ); // FIXME: get last dot not first
            if(icon[0] !='/' && dot )
                *dot = '\0';
        }
        DBG("icon: %s\n", icon);
        for (tmp = cats; *tmp; tmp++) {
            GtkWidget **menu, *mi;

            DBG("cat: %s\n", *tmp);
            if (!(menu = g_hash_table_lookup(m->ht, tmp[0])))
                continue;

            mi = gtk_image_menu_item_new_with_label(title);
            menu_item_set_image(mi, icon, icon, 22, 22);

            /* exec str is referenced as mi's object data and will be automatically fried 
             * upon mi's destruction. it was allocated by g_key_get_string */
            g_signal_connect(G_OBJECT(mi), "activate", (GCallback)spawn_app, exec);
            g_object_set_data_full(G_OBJECT(mi), "exec", exec, g_free);

            if (!(*menu))
                *menu = gtk_menu_new();
            g_object_set_data_full(G_OBJECT(mi), "item-name", title, g_free);
            _menu_shell_insert_sorted(GTK_MENU_SHELL(*menu), mi, title);
            //gtk_menu_shell_prepend(GTK_MENU_SHELL(*menu), mi);
            gtk_widget_show_all(mi);
            DBG("added =======================================\n");
            break;
        }
        g_free(icon);
    free_exec:
    free_cats:
        g_strfreev(cats);
    }
    g_key_file_free(file);
    g_dir_close(dir);
    g_chdir(cwd);
    g_free(cwd);
    RET();
}

/* Builds Free Desktop Org (fdo) menu. First, all application directories are
 * scanned to populate category menus. After that, all non-empty category menus
 * are connected as sub-menus to main (system) menu
 */
void
make_fdo_menu(plugin_instance *p, GtkWidget *menu)
{
    const char** sys_dirs = (const char**)g_get_system_data_dirs();
    int i;
    gchar *path;
    menu_priv *m = (menu_priv *) p;

    ENTER;
    m->ht = g_hash_table_new(g_str_hash, g_str_equal);
    for (i = 0; i < G_N_ELEMENTS(main_cats); i++) {
        g_hash_table_insert(m->ht, main_cats[i].name, &main_cats[i].menu);
        main_cats[i].menu = NULL;
        if (g_hash_table_lookup(m->ht, &main_cats[i].name))
            DBG("%s not found\n", main_cats[i].name);
    }

    for (i = 0; i < g_strv_length((gchar **)sys_dirs); ++i)    {
        path = g_build_filename(sys_dirs[i], app_dir_name, NULL );
        do_app_dir(p, path);
        g_free(path);
    }
    path = g_build_filename(g_get_user_data_dir(), app_dir_name, NULL);
    do_app_dir(p, path);
    g_free(path);
    //build menu
    for (i = 0; i < G_N_ELEMENTS(main_cats); i++) {
        GtkWidget *mi;
        gchar *name;

        if (main_cats[i].menu) {
            name = main_cats[i].local_name ? main_cats[i].local_name : main_cats[i].name;
            mi = gtk_image_menu_item_new_with_label(name);
            menu_item_set_image(mi, main_cats[i].icon, NULL, 22, 22);

            gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), main_cats[i].menu);
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
            gtk_widget_show_all(mi);
            gtk_widget_show_all(main_cats[i].menu);
        }
    }
    g_hash_table_destroy(m->ht);
    RET();
}

#endif 

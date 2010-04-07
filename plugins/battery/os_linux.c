
#define LEN 100
#define PROC_ACPI "/proc/acpi/battery/"

static gboolean
get_token_bool(gchar *buf, gchar *token, gboolean *value)
{
    ENTER;
    RET(FALSE);
}

static gboolean
get_token_int(gchar *buf, gchar *token, gint *value)
{
    ENTER;
    RET(FALSE);
}

static gboolean
read_proc(battery_priv *c, GString *path)
{
    int len, dcap, rcap;
    gchar *buf;
    gboolean ret, exist, charging;

    ENTER;
    len = path->len;
    
    g_string_append(path, "info");
    ret = g_file_get_contents(path->str, &buf, 0, NULL);
    g_string_truncate(path, len);
    if (!ret)
        RET(FALSE);
    ret = get_token_bool(buf, "present:", &exist)
        && exist && get_token_int(buf, "design capacity:", &dcap);
    g_free(buf);
    if (!ret)
        RET(FALSE);
    
    g_string_append(path, "state");
    ret = g_file_get_contents(path->str, &buf, 0, NULL);
    g_string_truncate(path, len);
    if (!ret)
        RET(FALSE);
    ret = get_token_bool(buf, "present:", &exist)
        && exist
        && get_token_int(buf, "remaining capacity:", &rcap)
        && get_token_int(buf, "charging state:", &charging);
    g_free(buf);
    if (!ret)
        RET(FALSE);
    DBG("battery=%s\ndesign capacity=%d\nremaining capacity=%d\n",
        path, dcap, rcap);

    if (!(dcap >= rcap && dcap > 0 && rcap >= 0))
        RET(FALSE);
    
    c->exist = TRUE;
    c->charging = charging;
    c->level = (int) ((gfloat) rcap * 100 / (gfloat) dcap);
    RET(TRUE);
}

static void
battery_update_os(battery_priv *c)
{
    
    GString *path;
    int len;
    GDir *dir;
    gboolean ret;
    const gchar *file;
    
    ENTER;
    c->exist = FALSE;
    path = g_string_sized_new(200);
    g_string_append(path, PROC_ACPI);
    len = path->len;
    if (!(dir = g_dir_open(path->str, 0, NULL)))
        goto out;
    
    while ((file = g_dir_read_name(dir))) {
        if (!g_file_test(file, G_FILE_TEST_IS_DIR))
            continue;
        g_string_append(path, file);
        ret = read_proc(c, path);
        g_string_truncate(path, len);
        if (ret)
            break;
    }    
    g_dir_close(dir);
        
out:
    g_string_free(path, TRUE);
}


#include <ctype.h>
#include <glib.h>
#include "profile.h"
#include "dbg.h"
#include "config.h"

int
get_line(FILE *fp, line *s)
{
    gchar *tmp, *tmp2;

    ENTER;
    s->type = LINE_NONE;
    if (!fp)
        RET(s->type);
    while (fgets(s->str, LINE_LENGTH, fp)) {
        g_strstrip(s->str);

        if (s->str[0] == '#' || s->str[0] == 0) {
            continue;
        }
        DBG( ">> %s\n", s->str);
        if (!g_ascii_strcasecmp(s->str, "}")) {
            s->type = LINE_BLOCK_END;
            break;
        }

        s->t[0] = s->str;
        for (tmp = s->str; isalnum(*tmp); tmp++);
        for (tmp2 = tmp; isspace(*tmp2); tmp2++);
        if (*tmp2 == '=') {
            for (++tmp2; isspace(*tmp2); tmp2++);
            s->t[1] = tmp2;
            *tmp = 0;
            s->type = LINE_VAR;
        } else if  (*tmp2 == '{') {
            *tmp = 0;
            s->type = LINE_BLOCK_START;
        } else {
            ERR( "parser: unknown token: '%c'\n", *tmp2);
        }
        break;
    }
    RET(s->type);

}

int
get_line_as_is(FILE *fp, line *s)
{
    gchar *tmp, *tmp2;

    ENTER;
    if (!fp) {
        s->type = LINE_NONE;
        RET(s->type);
    }
    s->type = LINE_NONE;
    while (fgets(s->str, LINE_LENGTH, fp)) {
        g_strstrip(s->str);
        if (s->str[0] == '#' || s->str[0] == 0)
        continue;
        DBG( ">> %s\n", s->str);
        if (!g_ascii_strcasecmp(s->str, "}")) {
            s->type = LINE_BLOCK_END;
            DBG( "        : line_block_end\n");
            break;
        }
        for (tmp = s->str; isalnum(*tmp); tmp++);
        for (tmp2 = tmp; isspace(*tmp2); tmp2++);
        if (*tmp2 == '=') {
            s->type = LINE_VAR;
        } else if  (*tmp2 == '{') {
            s->type = LINE_BLOCK_START;
        } else {
            DBG( "        : ? <%c>\n", *tmp2);
        }
        break;
    }
    RET(s->type);

}

FILE *
get_profile_file(gchar *profile, char *perm)
{
    gchar *fname;
    FILE *fp;
    int created = 0;
    
    ENTER;
    LOG(LOG_INFO, "Loading profile '%s'\n", profile);
try:
    fname = g_build_filename(g_get_user_config_dir(),
        "fbpanel", profile, NULL);
    fp = fopen(fname, perm);
    LOG(LOG_INFO, "Trying %s: %s\n", fname, fp ? "ok" : "not found");
    g_free(fname);
    if (fp)
        RET(fp);

    if (!fp && !created++)
    {
        gchar *cmd;

        cmd = g_strdup_printf("%s %s", LIBEXECDIR "/fbpanel/make_profile",
            profile);
        g_spawn_command_line_sync(cmd, NULL, NULL, NULL, NULL);
        g_free(cmd);
        goto try;
    }
    if (!fp)
        ERR("Can't open profile %s\n", profile);
    RET(NULL);
}

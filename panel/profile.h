#ifndef PROFILE_H
#define PROFILE_H

#include <stdio.h>

enum { LINE_NONE, LINE_BLOCK_START, LINE_BLOCK_END, LINE_VAR };

#define LINE_LENGTH 256
typedef struct {
    int type;
    gchar str[LINE_LENGTH];
    gchar *t[2];
} line;


int get_line(FILE *fp, line *s);
int get_line_as_is(FILE *fp, line *s);
FILE *get_profile_file(char *profile, char *perm);

#endif

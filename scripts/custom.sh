

#########################
## Custom Settings     ##
#########################

# Note 1: PWD will be that of configure, not scripts directory, so to run script from
#         this directory refer it as 'scripts/name'
# Note 2: values will be evaluated in the same order they were added, so
#         if you want libdir's default value to be '$eprefix/lib', add it
#         after prefix

# variable reference
#add_var user "test variable" joe
#add_var home "test variable" '/home/$user'

#add_var rfs "rfs" /tmp/rfs
#add_var glib_cflags "glib cflags" '`RFS=$rfs scripts/pc.sh --cflags glib-2.0`'
#add_var glib_libs "glib libs" '`RFS=$rfs scripts/pc.sh --libs glib-2.0`'

# autodetection: default value as output of command
#add_feature flag "some flag" '`echo enabled`'
add_var gtk_cflags "gtk cflags" '`scripts/pc.sh --cflags gtk+-2.0`'
add_var gtk_libs "gtk libs" '`scripts/pc.sh --libs gtk+-2.0`'
add_var gmodule_libs "gtk libs" '`scripts/pc.sh --libs gmodule-2.0`'
#add_var endianess "detect endianess (big or little)" '`scripts/endianess.sh`'
add_var os "detect OS flavour" '`uname -s | tr [:lower:] [:upper:]`'
add_var version "package version" '`cat version`'


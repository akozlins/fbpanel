#/bin/bash

function info()
{
    cat <<EOF
Create gettext translation files (*.po) with strings marked
with _() or c_() macros. Script scans all C files in current dir and down.

EOF
}

function help()
{
    cat <<EOF
Parameters
   package - package name
   version - package version
   destdir - dir to save resulted POT and PO files (default is po/ )
   locale - full locale name, e.g ru_RU.UTF-8
For example:
   create_po.sh --package=fbpanel --version=6.2 --locale=ru_RU.UTF-8
EOF
}

function prn_var ()
{
    eval echo \"$1=\$$1\"
}

function error ()
{
    echo "$@"
    exit 1
}

while [ $# -gt 0 ]; do
    if [ "$1" == "--help" ]; then
        info
        help
        exit 0
    fi
    if [ "${1:0:2}" != "--" ]; then
        error "$1 - not a parameter"
    fi
    tmp="${1:2}"
    var=${tmp%%=*}
    val=${tmp#*=}
    if [ "$var" == "$val" ]; then
        error "$1 - not a parameter"
    fi
    eval "$var=\"$val\""
    shift
done
[ -z "$package" ] && error "No package set"
[ -z "$version" ] && error "No version set"
[ -z "$locale" ] && error "No locale set"
[ -z "$destdir" ] && destdir=po

# code
pot=$destdir/$package.pot
list=/tmp/list-$$
mkdir -p $destdir
find . -name "*.c" -o -name "*.h" > $list

# Create POT file and set charset to be UTF-8
xgettext --package-name=$package --package-version=$version \
    --default-domain=$package --from-code=UTF-8 \
    --force-po -k_ -kc_ -f $list -o - |\
sed '/^"Content-Type:/ s/CHARSET/UTF-8/' > $pot

echo Created $pot

# create language translation files, POs
msginit --no-translator --locale=$locale --input=$pot -o $destdir/$locale.po




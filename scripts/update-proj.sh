#!/bin/bash

tar=`\ls -1 ~/src/fborg/proj-*.tbz2 | sort -ur | head -n 1`
csh=scripts/custom.sh

if [ ! -f $csh ]; then
    echo "run this script frmo topdir directory"
    exit 1
fi

cp  $csh $csh-`date +%Y-%m-%d-%H:%M:%S`
tar --strip-components=1 --exclude doc --exclude src \
    --keep-newer-files -xvf $tar


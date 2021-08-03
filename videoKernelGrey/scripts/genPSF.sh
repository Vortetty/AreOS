#!/bin/bash
set -o errexit

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <ttf file> [points, default 12]" >&2
    exit 1
fi
if ! [ -e "$1" ]; then
    echo "File \"$1\" not found" >&2
    exit 1
fi
if ! [ -f "$1" ]; then
    echo "\"$1\" is not a file" >&2
    exit 1
fi

if [ "$#" -eq 2 ]; then
    points=$2
else
    points=12
fi

echo "If you get \`/usr/bin/bdf2psf: font.bdf: the width is not integer number.\` below, then go into \`font.bdf\` and round \`AVERAGE_WIDTH\` to the nearest multiple of 10 then re-run this"
if ! [ -e "font.bdf" ]; then
    otf2bdf -p $points -r 96 -o font.bdf $1
fi
bdf2psf --fb font.bdf /usr/share/bdf2psf/standard.equivalents /usr/share/bdf2psf/fontsets/Uni2.512 512 src/font.psf
rm font.bdf

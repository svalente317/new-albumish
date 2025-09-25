#!/bin/sh
for file in `find . -name '*.san'`; do
    expand -t 4 $file | sed 's/[[:space:]]*$//' > /tmp/eraseme
    cmp -s $file /tmp/eraseme || mv /tmp/eraseme $file
done

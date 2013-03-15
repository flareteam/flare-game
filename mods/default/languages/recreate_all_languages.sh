#!/bin/sh
# Run this script in the language directory to update the pot and
# all *.po files

xgettext --no-wrap --keyword=get -o engine.pot ../../../src/*.cpp

for f in $(ls *.po) ; do
	echo "Processing $f"
	msgmerge -U --no-wrap $f engine.pot
done

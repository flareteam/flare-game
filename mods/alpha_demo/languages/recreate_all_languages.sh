#!/bin/sh
# Run this script in the language directory to update the pot and all *.po files for mods

./xgettext.py

for f in $(ls *.po) ; do
	echo "Processing $f"
	msgmerge -U --no-wrap $f data.pot
done


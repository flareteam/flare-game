#!/bin/sh
# Run this script in the language directory to update the pot and all *.po files

# For the engine
# To generate the appropriate .pot file, you need to run the following command from the languages directory:
if [ -e engine.pot ] ; then
xgettext --keyword=get -o engine.pot ../../../src/*.cpp

# xgettext doesn't allow defining a charset, but we want UTF-8 across the board
sed -i "s/charset=CHARSET/charset=UTF-8/" engine.pot

# To update existing .po files, you need to run the following command from the languages directory:
# msgmerge -U -N <name_of_.po_file> <name_of_.pot_file>

	for f in $(ls engine.*.po) ; do
	echo "Processing $f"
	msgmerge -U -N $f engine.pot
done
fi

if [ -e data.pot ] ; then
	# For mods:
	./xgettext.py

	for f in $(ls data.*.po) ; do
		echo "Processing $f"
		msgmerge -U -N $f data.pot
	done
fi

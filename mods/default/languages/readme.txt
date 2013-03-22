#!/bin/sh
# This is both readme and an executable script at once.
# Run this script in the language directory to update the pot and all *.po files

# For the engine
# To generate the appropriate .pot file, you need to run the following command from the languages directory:
if [ -e engine.pot ] ; then
	xgettext --no-wrap --keyword=get -o engine.pot ../../../src/*.cpp

	# To update existing .po files, you need to run the following command from the languages directory:
	# msgmerge -U --no-wrap <name_of_.po_file> <name_of_.pot_file>

	for f in $(ls engine.*.po) ; do
		echo "Processing $f"
		msgmerge -U --no-wrap $f engine.pot
	done
fi

if [ -e data.pot ] ; then
	# For mods:
	./xgettext.py

	for f in $(ls data.*.po) ; do
		echo "Processing $f"
		msgmerge -U --no-wrap $f data.pot
	done
fi



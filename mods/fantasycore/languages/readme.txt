To generate the appropriate .pot files, you need to run the following commands from the languages directory:

For engine:
xgettext --no-wrap --keyword=get -o engine.pot ../../../src/*.cpp

For mods:
./xgettext.py

To update existing .po files, you need to run the following command from the languages directory:

msgmerge -U --no-wrap <name_of_.po_file> <name_of_.pot_file>

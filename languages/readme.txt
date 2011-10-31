To generate the appropriate .pot files, you need to run the following commands from the languages directory:

xgettext --keyword=get -o engine.pot ../src/*.cpp
./xgettext.py

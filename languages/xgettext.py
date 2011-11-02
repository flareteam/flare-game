#! /usr/bin/python
import os
import datetime

keys = set()
now = datetime.datetime.now()
header = r'''# Copyright (C) 2011 Clint Bellanger
# This file is distributed under the same license as the FLARE package.
#
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
msgid ""
msgstr ""
"Project-Id-Version: PACKAGE VERSION\n"
"Report-Msgid-Bugs-To: \n"
"POT-Creation-Date: {now}\n"
"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
"Language-Team: LANGUAGE <LL@li.org>\n"
"Language: \n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=CHARSET\n"
"Content-Transfer-Encoding: 8bit\n"

'''

# this extracts translatable strings from the flare data file
def extract(filename):
    infile = open(filename, 'r')
    for line in infile.readlines():
        if line.startswith('name='):
            keys.add(line[5:].strip('\n'))
        elif line.startswith('power_desc='):
            keys.add(line[11:].strip('\n'))
        elif line.startswith('description='):
            keys.add(line[12:].strip('\n'))
        elif line.startswith('title='):
            keys.add(line[6:].strip('\n'))
        elif line.startswith('msg='):
            keys.add(line[4:].strip('\n'))

# this writes the list of keys to a gettext .po file
def save(filename):
    outfile = open('data.pot', 'w')
    outfile.write(header.format(now=now.strftime('%Y-%m-%d %H:%M+%z')))

    for line in keys:
        outfile.write("msgid \"{line}\"\n".format(line=line))
        outfile.write("msgstr \"\"\n\n")

extract('../items/items.txt')
extract('../powers/powers.txt')
for filename in os.listdir('../enemies'):
    extract('../enemies/{enemy}'.format(enemy=filename))
save('data.pot')

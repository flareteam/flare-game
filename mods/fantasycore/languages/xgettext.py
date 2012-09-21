#! /usr/bin/python
import os
import datetime

keys = []
comments = []
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
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"

'''

# this extracts translatable strings from the flare data file
def extract(filename):
    if os.path.exists(filename):
        infile = open(filename, 'r')
        triggers = ['msg', 'him', 'her', 'you', 'name', 'title', 'tooltip',
                'power_desc', 'quest_text', 'description', 'item_type', 'slot_name', 'tab_title', 'resist', 'currency_name']
        for i,line in enumerate(infile):
            for trigger in triggers:
                if line.startswith(trigger + '='):
                    aString = filename
                    aString += ':'
                    aString += str(i+1)
                    comments.append(aString)
                    del aString
                    keys.append(line[line.find('=') + 1:].strip('\n').replace("\"", "\\\"").rstrip())
            # handle the special case: bonus={stat},{value}
            if line.startswith('bonus='):
                aString = filename
                aString += ':'
                aString += str(i+1)
                comments.append(aString)
                del aString
                keys.append(line[line.find('=') + 1: line.find(',')].rstrip())

# this removes duplicates from keys in a clean way (without screwing up the order)
def remove_duplicates():
    global comments
    global keys
    tmp = []
    tmp_c = []
    for node_c,node in zip(comments,keys):
        if node not in tmp:
            tmp_c.append(node_c)
            tmp.append(node)
    comments = tmp_c
    keys = tmp

# this writes the list of keys to a gettext .po file
def save(filename):
    outfile = open('data.pot', 'w')
    outfile.write(header.format(now=now.strftime('%Y-%m-%d %H:%M+%z')))
    remove_duplicates()
    for line_c,line in zip(comments,keys):
        outfile.write("#: {line}\n".format(line=line_c))
        outfile.write("msgid \"{line}\"\n".format(line=line))
        outfile.write("msgstr \"\"\n\n")

# this extracts the quest files from the quests directory
def get_quests():
    quests = set()
    infile = open('../quests/index.txt', 'r')
    for line in infile.readlines():
        quests.add(line.strip('\n'))
    infile.close()
    return quests



# HERE'S THE MAIN EXECUTION
extract('../items/items.txt')
extract('../items/types.txt')
extract('../menus/inventory.txt')
extract('../menus/powers.txt')
extract('../powers/powers.txt')
extract('../engine/elements.txt')
extract('../engine/loot.txt')

if os.path.exists('../enemies'):
	for filename in os.listdir('../enemies'):
	    extract('../enemies/{enemy}'.format(enemy=filename))

if os.path.exists('../maps'):
	for filename in os.listdir('../maps'):
	    extract('../maps/{map}'.format(map=filename))

if os.path.exists('../quests'):
	for filename in get_quests():
	    extract('../quests/{quest}'.format(quest=filename))

if os.path.exists('../npcs'):
	for filename in os.listdir('../npcs'):
	    extract('../npcs/{npc}'.format(npc=filename))

save('data.pot')

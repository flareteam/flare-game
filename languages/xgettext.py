#! /usr/bin/python
import os
import datetime

keys = []
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
    triggers = ['msg', 'him', 'you', 'name', 'title', 'tooltip',
            'power_desc', 'quest_text', 'description']
    for line in infile.readlines():
        for trigger in triggers:
            if line.startswith(trigger + '='):
                keys.append(line[line.find('=') + 1:].strip('\n').replace("\"", "\\\""))

# this removes duplicates from keys in a clean way (without screwing up the order)
def remove_duplicates():
    global keys
    tmp = []
    for node in keys:
        if node not in tmp:
            tmp.append(node)
    keys = tmp

# this writes the list of keys to a gettext .po file
def save(filename):
    outfile = open('data.pot', 'w')
    outfile.write(header.format(now=now.strftime('%Y-%m-%d %H:%M+%z')))
    remove_duplicates()
    for line in keys:
        outfile.write("msgid \"{line}\"\n".format(line=line))
        outfile.write("msgstr \"\"\n\n")

# this extracts the quest files from the quests directory
#def get_quests():
#    quests = set()
#    infile = open('../quests/index.txt', 'r')
#    for line in infile.readlines():
#        quests.add(line.strip('\n'))
#    infile.close()
#    return quests

extract('../items/items.txt')
extract('../powers/powers.txt')
for filename in os.listdir('../enemies'):
    extract('../enemies/{enemy}'.format(enemy=filename))
# WE WON'T USE THESE UNTIL MODS ARE READY
#for filename in os.listdir('../maps'):
#    extract('../maps/{map}'.format(map=filename))
#for filename in get_quests():
#    extract('../quests/{quest}'.format(quest=filename))
#for filename in os.listdir('../npcs'):
#    extract('../npcs/{npc}'.format(npc=filename))
save('data.pot')

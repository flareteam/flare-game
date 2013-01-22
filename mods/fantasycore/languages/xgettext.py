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

POT_STRING = '''\
#: {comment}
msgid "{msgid}"
msgstr ""

'''

# this extracts translatable strings from the flare data file
def extract(filename):
    if not os.path.exists(filename):
        return
    infile = open(filename, 'r')
    triggers = [
        'msg', 'him', 'her', 'you', 'name', 'title', 'tooltip',
        'power_desc', 'quest_text', 'description', 'item_type',
        'slot_name', 'tab_title', 'resist', 'currency_name',
        'bonus', 'flavor',
    ]
    plain_text = [
        'msg', 'him', 'her', 'you', 'name', 'title', 'tooltip',
        'quest_text', 'description',
        ]
    for i, line in enumerate(infile, start=1):
        for trigger in triggers:
            if line.startswith(trigger + '='):
                line = line.split('=')[1]
                line = line.strip('\n')
                values = line.split(',')
                if (trigger in plain_text):
                   stat = line.replace("\"", "\\\"");
                elif len(values) == 1:
                   # {key}={value}
                   stat, = values
                elif len(values) == 2:
                   # bonus={stat},{value}
                   stat, value = values
                elif len(values) == 3:
                   # bonus={set_level},{stat},{value}
                   set_level, stat, value = values
                comment = filename + ':' + str(i)
                comments.append(comment)
                keys.append(stat.rstrip())

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
        outfile.write(POT_STRING.format(comment=line_c, msgid=line))

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
extract('../items/sets.txt')
extract('../menus/inventory.txt')
extract('../menus/powers.txt')
extract('../powers/powers.txt')
extract('../engine/elements.txt')
extract('../engine/loot.txt')

for folder in ['enemies', 'maps', 'quests', 'npcs']:
    target = os.path.join('..', folder)
    if os.path.exists(target):
        for filename in sorted(os.listdir(target)):
            extract(os.path.join(target, filename))

save('data.pot')

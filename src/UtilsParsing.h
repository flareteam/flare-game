/*
 *  UtilsParsing.h
 *  RPGEngine
 *
 *  Created by Clint Bellanger on 5/25/10.
 *  Copyright 2010 Clint Bellanger. All rights reserved.
 *
 */

#ifndef UTILS_PARSING_H
#define UTILS_PARSING_H

#include <string>
#include <stdlib.h>
#include <fstream>
using namespace std;

bool isInt(string s);
unsigned short xtoi(char c);
unsigned short xtoi(string hex);
char btox(bool b1, bool b2, bool b3, bool b4);
string trim(string s, char c);
string parse_section_title(string s);
void parse_key_pair(string s, string &key, string &val);
int eatFirstInt(string &s, char separator);
unsigned short eatFirstHex(string &s, char separator);
string eatFirstString(string &s, char separator);
string stripCarriageReturn(string line);
string getLine(ifstream &infile);

#endif

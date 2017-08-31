#include <bits/stdc++.h>
#include <vector>

#include "classdef.h"
#include "colormod.h"
using namespace std;
using namespace Color;

int strcnt(char s[], const char ch) {
	int c = 0;
	for(int i = 0; s[i] != '\0'; i++)
		if(s[i] == ch) c++;
	return c;
} 

Modifier red(FG_RED);
Modifier def(FG_DEFAULT);

bool echo(vector<char*> tokens) {
	vector<char*> flags;
	vector<char*> strings;
	for(vector<char*>::iterator i = tokens.begin() + 1; i != tokens.end(); i++)
		if((*i)[0] == '-') flags.push_back(*i);
		else strings.push_back(*i);
	// add more flags here
	bool no_newline = false;
	for(auto i: flags)
		if(strchr(i, 'n')) { no_newline = true; break; }
	
	char beg_char;
	for(auto i: strings) {
		
		if(i[0] == '\'' || i[0] == '\"') beg_char = i[0];
		else beg_char = NULL;
		
		if(strcnt(i, '\'') % 2 || strcnt(i, '\"') % 2) {
			cout<<red<<"shkell: Unbalanced quotes"<<def<<endl;
			return false;
		}
		
		char temp_str[strlen(i)]; int c = 0;
		for(int j = 1; j < strlen(i); j++)
			if(i[j] == beg_char && i[j - 1] != '\\')
				continue;
			else
				temp_str[c++] = i[j];
		while(c < strlen(i)) temp_str[c++] = NULL;
		cout<<temp_str<<" ";
	}
	if(!no_newline) cout<<endl;
	return true;
}
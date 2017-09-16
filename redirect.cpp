// handles file redirection
#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <utility> 

#include "colormod.h"
#include "classdef.h"

using namespace std;
using namespace Color;

#define FORWARD_SINGLE 62
#define FORWARD_APPEND 124
#define BACKWARD_SINGLE 60
#define BACKWARD_APPEND 120

// get the type of pipe, using simple hashing 
int get_type(char *s) {
	int c = 0;
	for(int i = 0; s[i] != '\0'; c += s[i++]);
		return c;
}


// handles the separation of the tokens and values
pair< vector< vector<char*> >, vector< pair<int, int> > >get_separated(vector<char*> tokens) {
	
	vector< vector<char*> > separated;
	vector< pair<int, int> > codes;
	vector<char*> cur_arg;

	// seperates it into tokens taking care of error codes
	for(auto i: tokens) {
		if (strstr((i), "<") || strstr((i), ">")  || strstr((i), "<<")  || strstr((i), ">>")) {
			if(strcmp((i), "<") == 0 || strcmp((i), ">") == 0  
				|| strcmp((i), "<<") == 0  || strcmp((i), ">>") == 0)
				codes.push_back(make_pair(1, get_type(i)));
			else codes.push_back(make_pair(i[0] - 48, get_type(i + 1)));
			if(cur_arg.size())
				separated.push_back(cur_arg);
			cur_arg.clear();
		}
		else cur_arg.push_back(i);
	}

	// pushing the last command
	separated.push_back(cur_arg);

	return make_pair(separated, codes);
} 


// checks for proper redirection
bool check_redirection(vector< vector<char*> > separated, vector< pair<int, int> > codes) {
	if(separated.size() != codes.size() + 1) return true;
	for(auto i: codes)
		if((i.first != 0 && i.first != 1 && i.first != 2)
			|| (i.second != FORWARD_SINGLE && i.second != FORWARD_APPEND
				&& i.second != BACKWARD_SINGLE && i.second != BACKWARD_APPEND)) 
			return true;
		return false;
}


// the main function that handles the redirection
int handle_redirect(vector<char*> tokens) {
	Modifier red(FG_RED);
	Modifier def(FG_DEFAULT);

	auto i = get_separated(tokens);
	vector< vector<char*> > separated = i.first;
	vector< pair<int, int> > codes = i.second;

	if(check_redirection(separated, codes)) {
		cout<<red<<"Invalud redirection"<<def<<endl;
		return false;
	}

	for(auto i: separated) {
		cout<<endl<<"Command "<<" : ";
		for(auto j: i) cout<<j<<" ";
	}
	for(auto i: codes) cout<<i.first<<" : "<<i.second<<endl;

	// execute and pipe here
	vector< pair<int, int> >::iterator c = codes.begin();
	for(auto i: separated) {
		for(auto j: i) {

		}
	}
	return true;
}	
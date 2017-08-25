#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>


#include "classdef.h"
#include "colormod.h"
using namespace Color;
using namespace std;

BaseDetails::BaseDetails() {

	register struct passwd *pw;
	register uid_t uid;
	uid = geteuid();
	pw = getpwuid(uid);
	if (pw)  strcpy(user_name, pw->pw_name);

	getcwd(pwd1, PWD_LENGTH);
	gethostname(host_name, SYSTEM_NAME_LENGTH);
}

// updates the pwd everytime
void BaseDetails::update() { 
	getcwd(pwd1, PWD_LENGTH);
	sub_home();
}

// replaces the home path with '~'
void  BaseDetails::sub_home() {
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL) 
		homedir = getpwuid(getuid())->pw_dir;
	bool is_same = true;
	for(int i = 0; i < strlen(homedir); i++) {
		if(pwd1[i] != homedir[i]) {
			is_same = false;
			break;
		}
	}
	if(is_same && strlen(pwd1) > strlen(homedir)) {
    	// - 1 because of ~ repr
		int repl = strlen(homedir) - 1;
		for(int i = 1; i < strlen(pwd1) - repl; i++) 
			pwd1[i] = pwd1[repl + i];
		for(int i = repl + 1; i < strlen(pwd1); pwd1[i++] = '\0');
			pwd1[0] = '~';
	}
	else if(is_same && strlen(pwd1) == strlen(homedir))
		strcpy(pwd1, "~");

}
char* BaseDetails::get_user() { return user_name; }
char* BaseDetails::get_host() { return host_name; }
char* BaseDetails::get_cwd()  { return pwd1; 	  }

void BaseDetails::print_term() {
	Modifier bg_green(BG_GREEN);
	Modifier bg_blue(BG_BLUE);
	Modifier fg_green(FG_GREEN);
	Modifier fg_blue(FG_BLUE);

	Modifier fg_def(FG_DEFAULT);
	Modifier bg_def(BG_DEFAULT);
	Modifier fg_black(FG_BLACK);
	cout<<bg_blue<<fg_black<<get_user()<<"@"<<get_host()<<" "<<
	bg_green<<fg_blue<<"▶ "<<fg_black<<get_cwd()<<" "<<
	bg_def<<fg_green<<"▶ "<<bg_def<<fg_def;
}

#include <bits/stdc++.h>
#include <unistd.h>
#include "classdef.h"

using namespace std;

BaseDetails::BaseDetails() {
	getlogin_r(user_name, USER_NAME_LENGTH);
	getcwd(pwd1, PWD_LENGTH);
	gethostname(host_name, SYSTEM_NAME_LENGTH);
}

void BaseDetails::update() { getcwd(pwd1, PWD_LENGTH); }


char* BaseDetails::get_user() { return user_name; }
char* BaseDetails::get_host() { return host_name; }
char* BaseDetails::get_cwd()  { return pwd1; 	  }



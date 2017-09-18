#include <bits/stdc++.h>
#include <unistd.h>
#include "classdef.h"

using  namespace std;

char* int_to_string(int pid){

	char* buf = (char*)malloc(PID_LENGTH);
	sprintf(buf,"%d",pid);
	return buf;
}

void pinfo(vector<char*> tokenized){
	char** args = (char**) malloc(tokenized.size() * sizeof(char*));
	int ic = 0;
	for(auto i: tokenized) args[ic++] = i;
	args[tokenized.size()] = NULL;
		


	char stat_file[FILE_NAME],exe_file[FILE_NAME];
	char line[STAT_LENGTH],execute_path[PWD_LENGTH];

	strcpy(stat_file,"/proc/");
	if (tokenized.size() == 1)
		strcat(stat_file,int_to_string(getpid()));
	else if (tokenized.size() ==2)
		strcat(stat_file,args[1]);
	
	strcat(exe_file,stat_file);
	strcat(stat_file,"/stat");

	FILE* inputFile = fopen(stat_file,"r");
	if(inputFile == NULL)
		cout<<"Process doesnt exist"<<endl;	
	else
	{
		
		fgets(line, sizeof(line),inputFile);

		char* stat_param[100];
		stat_param[0] = strtok(line," ");
		for(int i=0;stat_param[i] != NULL;i++)
			stat_param[i+1] = strtok(NULL," ");

		cout<<"PID:"<<stat_param[0]<<endl;
		cout<<"Name:"<<stat_param[1]<<endl;
		cout<<"State:"<<stat_param[2]<<endl;
		cout<<"Virtual Memory:"<<stat_param[22]<<endl;
	
		strcat(exe_file,"/exe");
		if(readlink(exe_file, execute_path, PWD_LENGTH)< 0)
			cout<<"Unable to locate the executable path"<<endl;
		else
			cout<<"Executable Path:"<<execute_path<<endl;	
		exe_file[0] = '\0';

		}
}
#include <unistd.h>
#include <bits/stdc++.h> 
#include "classdef.h"
#include <sys/wait.h>

using namespace std;

int pipeline(char* cmd,int is_pipe){
	// piped commands		

	vector<char*> pipe_commands; 
	char* temp=strtok(cmd,"|");
	while(temp != NULL){

		temp = remove_padding(temp);
		
		if(temp == NULL)
			break;

		if(strlen(temp))
			pipe_commands.push_back(temp);

		temp = strtok(NULL,"|");
	}

	int in=0,initial_stdin,initial_stdout;
	int fd[2];
	initial_stdin=dup(0);
	initial_stdout=dup(1);


	temp = pipe_commands.back();
	pipe_commands.pop_back();

	for (auto c:pipe_commands)
	{
		if(c == NULL)
			break;
		if(pipe(fd)<0)
			perror("Piping Failed");

		int pid=fork();

		if(pid < 0)
			perror("Fork Error");


		else if(pid ==0)
		{
			if(in!=0)
			{
				dup2(in,0);
				close(in);
			}

			dup2(fd[1],1);
			close(fd[1]);
		
			single_command(c);
			exit(0);

		}

		else{
			close(fd[1]);
			wait(NULL);
			in = fd[0];
			}

	}

	if(in!=0)
		{dup2(in,0);close(in);}
	single_command(temp);

	dup2(initial_stdin,0);
	if(initial_stdin!=0)
		close(initial_stdin);

	dup2(initial_stdout,1);
	if(initial_stdout!=1)
		close(initial_stdout);

	return 0;

}
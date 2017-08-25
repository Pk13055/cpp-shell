// OS Assignment 2 - Interactive Terminal 
// 		-by Shubh Maheshwari 20161170
//          Pratik Kamble 

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>

int main()
{
	char user_name[64],system_name[64],cmd[202],current_directory[1024];

	//returns the user name(eg.shubh) of the PC 
	getlogin_r(user_name,64);

	// returns the system name(eg.Jarvis) of the PC
	getcwd(current_directory,1024);

	// returns the current directory where the program was executed
	gethostname(system_name,64);


	// The terminal will take more than command hence in each loop 
	// a child process is created. The child process executes the command. 
	// the parent waits for the child process to complete and then lets the
	// next iteration of loop to run 

	while(1)
	{	
		int pid = fork();

		if(pid == 0 ){
				printf("%s@%s:%s =>",user_name,system_name,current_directory);
				scanf("%s",cmd);
				execlp(cmd,cmd,(char *)NULL);	
		}
		
		else
			wait(NULL); //parent process is expected to wait otherwise in the parent process the new iteration will begin 
						// before the completion of	child process and thus it would keep on forking.

	}

	return 0;
}
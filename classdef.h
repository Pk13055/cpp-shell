#include <vector>
using namespace std;

#define USER_NAME_LENGTH 200
#define SYSTEM_NAME_LENGTH 200
#define COMMAND_LENGTH 500
#define PWD_LENGTH 2048
#define JOB_NAME 200

class BaseDetails {
	char user_name[USER_NAME_LENGTH];
	char host_name[SYSTEM_NAME_LENGTH];
	char pwd1[PWD_LENGTH];
	char home_dir[PWD_LENGTH / 2];
	void sub_home();
	char* get_host();
	char* get_user();
public:
	void print_term();
	char* get_cwd();
	BaseDetails();
	void update();
};

class Process {
	char name[JOB_NAME];
	int job_type, status;
	pid_t pid;
public:
	pid_t get_pid() { return pid; }
	int get_type() { return job_type; }
	void set_pid(pid_t t) { pid = t; }
	void set_name(char s[]) { 
		if(strlen(s) < JOB_NAME) 
			strcpy(name, s);
		else
			strncpy(name, s, (size_t) JOB_NAME);
	}
	void set_job(int type) { job_type = type; }
};

extern map<pid_t, Process> all_proc; 

char* remove_padding(char s[]);
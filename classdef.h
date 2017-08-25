#include <vector>
using namespace std;

#define USER_NAME_LENGTH 200
#define SYSTEM_NAME_LENGTH 200
#define COMMAND_LENGTH 500
#define PWD_LENGTH 2048


class BaseDetails {
	char user_name[USER_NAME_LENGTH];
	char host_name[SYSTEM_NAME_LENGTH];
	char pwd1[PWD_LENGTH];
public:
	BaseDetails();
	void update();
	char* get_host();
	char* get_user();
	char* get_cwd();
};
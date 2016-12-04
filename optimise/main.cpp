#include <iostream>
#include <map>

#include "ICommand.h"
#include "Commands.h"

using namespace std;
#if 1
//static map<string, ICommand*> mCommands;
static ICommand* cpufreq = NULL;
static ICommand* cgroup = NULL;
void __attribute__((constructor)) init() {
#if 0
	string cpufreqName("cpufreq");
	CpufreqCmd* cpufreq = new CpufreqCmd(cpufreqName.c_str());

	mCommands[cpufreqName] = cpufreq;
#endif
    cpufreq = new CpufreqCmd("cpufreq");
    cgroup = new CgroupCmd("cgroup");
}

void __attribute((destructor)) destroy() {
    if (cpufreq)
		delete cpufreq;

	if (cgroup)
		delete cgroup;
}
#endif

int parse(char* msg, char **args, int len) {
    const char* delim = " ";
	int count = 0;

	char *str = strtok(msg, delim);
	args[count++] = strdup(str);
	
	while(str != NULL) {
	  cout<< str << endl;
	  str = strtok(NULL, delim);
	  if (str)
	      args[count++] = strdup(str);

	  if (count > len)
	  	break;
	}

    return count;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "input at least 3 arguments" << endl;
		return -1;
	}

	char msg[] = {"cpufreq governor performance"};
	int len = 3;
    char *args[3];

	parse(msg, args, len);
	for (int i = 0; i < len; i++) {
		if (args[i])
            free(args[i]);
	}
	
	if (!strcmp(argv[1], cpufreq->getCmdName()))
		cpufreq->onCommand(argc,argv);
    else if (!strcmp(argv[1], cgroup->getCmdName()))
		cgroup->onCommand(argc, argv);
	
	return 0;
}


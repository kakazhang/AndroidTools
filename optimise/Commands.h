#ifndef COMMANDS_H
#define COMMANDS_H

#include "ICommand.h"

/**
* CpufreqCmd includes:
* 1) change cpufreq governor
* 2) tune cpufreq cpuload and time rate, and so on.
*/
class CpufreqCmd : public ICommand {
public:
    CpufreqCmd(const char* cmd);
    virtual ~CpufreqCmd();

    virtual void onCommand(int argc, char *args[]);
private:
   int changeGovernor(int argc, char **args);
   int tuneMinspeedLoad(int argc, char **args);
};

class CgroupCmd : public ICommand {
public:
    CgroupCmd(const char* cmd);
    virtual ~CgroupCmd();

    virtual void onCommand(int argc, char *args[]);
private:
    int tuneLimitMemory(int argc, char **args);
};

class ReclaimCmd : public ICommand {
public:
    ReclaimCmd(const char* cmd);
    virtual ~ReclaimCmd();

    virtual void onCommand(int argc, char *args[]);
private:
    void forceReclaim(int argc, char **args);
};

#endif

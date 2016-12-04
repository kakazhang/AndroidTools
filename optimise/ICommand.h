#ifndef ICOMMAND_H
#define ICOMMAND_H

class ICommand {
public:
    ICommand(const char* name)
		: mCmdName(name){

	}

	virtual ~ICommand() {

	}
	
    virtual void onCommand(int argc, char *args[]) = 0;
	
    const char* getCmdName() { return mCmdName; }
private:
	const char* mCmdName;
};

#endif

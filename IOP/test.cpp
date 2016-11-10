#include <iostream>
#include <stdlib.h>
#include <binder/IServiceManager.h>
#include <utils/String16.h>
#include <utils/String8.h>
#include "IFileService.h"

using namespace std;
using namespace android;

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "Too few arguments" << endl;
		exit(0);
	}
	
	sp<IFileService> fs;
	
    const sp<IServiceManager> sm(defaultServiceManager());
    if (sm != NULL) {
      const String16 name("FileService");
      fs = interface_cast<IFileService>(sm->getService(name));
      
      String16 *files = NULL;
	    int len = 0;
	    
	    fs->getFilesForCmdline(argv[1], &files, &len);
	    if (len > 0) {
	    	for (int i = 0; i < len; i++) {
                String8 str(files[i]);
	    	    cout << "get:" << str << endl;
	    	}
	    } else {
	        cout << "get nothing" << endl;
	    }
    } else { cout << "no service" << endl;}
	
	return 0;
}

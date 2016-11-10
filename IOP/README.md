# IOP
Android: A native service used for detecting all the files opened by processes,and offering an opportunity to preload files
Functions:
1. IFileService.h IFileService.cpp 
    Define BnFileService and BpFileService
    
2. FileService.h FileService.cpp main.cpp
    Native service used to get opened files by processes

3. GetProcessFiles.h GetProcessFiles.cpp
    Loop to read all the files opened by processes. This needs root permission.
    
4. test.cpp
     test to read from FileService. So we can decide to map files into memory to decreasing IO .

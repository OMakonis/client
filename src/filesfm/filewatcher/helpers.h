#pragma once

#ifndef UNICODE
#define UNICODE 1
#endif

#include <string>

#include <windows.h>
#include "systemhandles.h"
#include "process.h"

using namespace std;

namespace FilesFM {
    namespace FileWatcher {

        class Helpers {
            public: 
                // Returns a file path from the passed handle
                static BOOL GetFilePathFromHandle(HANDLE hFile, wstring& outPath);
                
                // Gets all system handles
                static SYSTEM_HANDLE_INFORMATION* GetSystemHandles();

                // Opens the process and requests the necessary access rights
                static Process* OpenAndPrepareProcess(DWORD processId);
        };
    
    }
}

#pragma once
#define UNICODE 1

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <thread>


// Windows API
#include <windows.h>


// Models
#include "process.h"
#include "folder.h"
#include "file.h"

#include "systemhandles.h"
#include "callbackinterface.h"


using namespace std;


namespace FilesFM {
    namespace FileWatcher {

        class FileWatcher
        {
        private:
            bool _initialized = false;
            bool _isFirstRun = true;
            unordered_map<DWORD, Process*> _processes;
            BYTE _fileObjectType = 0;
            thread* _mainThread = NULL;

            // A list of paths that should be monitored
            unordered_set<wstring> _pathsToMonitor;

            // A list of process names that should be skipped
            unordered_set<wstring> _processNamesToSkip;

            // A list of paths that should be skipped
            unordered_set<wstring> _pathsToSkip;

            // A list of patterns that should be excluded from watched files
            list<wstring> _excludePatterns {
                L"*~",
                L"~$*",
                L".~lock.*",
                L"~*.tmp",
                L"*.~*",
                L"Icon\r*",
                L".DS_Store",
                L".ds_store",
                L"._*",
                L"Thumbs.db",
                L"desktop.ini",
                L".*.sw?",
                L".*.*sw?",
                L".TemporaryItems",
                L".Trashes",
                L".DocumentRevisions-V100",
                L".Trash-*",
                L".fseventd",
                L".apdisk",
                L".htaccess",
                L".directory",
                L"*.part",
                L"*.filepart",
                L"*.crdownload",
                L"*.kate-swp",
                L"*.gnucash.tmp-*",
                L".synkron.*",
                L".sync.ffs_db",
                L".symform",
                L".symform-store",
                L".fuse_hidden*",
                L"*.unison",
                L".nfs*",
                L"My Saved Places."
                };

            void _addProcess(Process* process);
            void _removeProcess(Process* process);
            unordered_map<DWORD, Process*>::iterator _removeProcess(unordered_map<DWORD, Process*>::iterator iterator);

        public:
            // Constructor
            FileWatcher();

            // Destructor
            ~FileWatcher();

            // File opened and closed callback class pointer
            CallbackInterface* CallbackClass = 0;

            // // File opened callback
            // void (*OnFilesOpened) (void* callbackClass, vector<File*>);

            // // File closed callback
            // void (*OnFilesClosed) (void* callbackClass, vector<File*>);

            // Holds the last created instance for OpenFileWatcher. Use with caution, if using multiple instances of this class!
            static FileWatcher* Instance;

            // Identifies wether the main thread is running
            bool Running = false;

            // Signals for all the threads to stop
            bool Terminate = false;

            // overrides _fileObjectType (this value is windows version and/or build specific, usually an internal compatibility function is being used) 
            BYTE FileObjectType = 0;

            // use throttling to reduce CPU usage
            BOOL UseThrottling = true;

            // how many handles to process before sleeping
            int ThrottleInterval = 200;		

            // in milliseconds
            int ThrottleDelay = 50;	

            // run the process
            void Run();

            // Start the main thread
            void Start();

            // Start the main thread
            void Stop();

            // All currently open files
            vector<File*> OpenFiles;

            // Add a path that should be skipped
            void AddPathToMonitor(wstring path);

            // Add a path that should be skipped
            void RemovePathToMonitor(wstring path);

            // Add a process name to skip
            void AddProcessNameToSkip(wstring processName);
            
            // Add a process name to skip
            void AddProcessNameToSkip(string processName);

            // Add a path that should be skipped
            void AddPathToSkip(wstring path);
        };

    }
}
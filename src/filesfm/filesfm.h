#pragma once
#define UNICODE 1

// Necessary for Win API includes to work correctly (setting version to Windows Vista, 0x0600)
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600


// Libraries
#include "commands.h"
#include "files.h"
#include "folders.h"
#include "filewatcher/filewatcher.h"
#include "messaging.h"
#include "logs.h"
#include "logger.h"
#include "shell_extensions.h"

using namespace std;

namespace FilesFM {
    class FilesFM
    {
        private:


        public:
            FilesFM();
            ~FilesFM();
            static FilesFM *Instance;

            // This is here to call the constructor of the class. The rest is being handled through static functions and members.
            Logger Logger;

            // The file watcher, the class that actually checks for opened/closed files
            FileWatcher::FileWatcher FileWatcher;

            // Holds open folders and helper functions
            Folders Folders {this}; 

            // Commands that can be send through the shell extension (right click menu)
            Commands Commands {this};

            // Holds open files and helper functions
            Files Files  {this};

            // Periodically checks for new messages from the server 
            Messaging Messaging {this};

            // Handles log file uploads
            Logs Logs {this};
            
            // Handles shell extensions
            ShellExtensions ShellExtensions;

            // Prepares the file watcher
            void SetupFileWatcher();
    };
}
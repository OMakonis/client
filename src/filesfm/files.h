#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "filesfm.fwd.h"
#include "filewatcher/file.h"
#include "filewatcher/callbackinterface.h"
#include "folder.h"
#include "file_action.h"

using namespace std;

namespace FilesFM {
    class Files : public FileWatcher::CallbackInterface {
        private:

        public:
            FilesFM* filesFM;

            Files(FilesFM* parent);
            ~Files();

            // Lock the specified file
            void LockFile(wstring absolutePath);

            // Unlock the specified file
            void UnlockFile(wstring absolutePath);

            // An aggregate function that can lock or unlock a file
            void LockOrUnlockFile(wstring absolutePath, bool lock);
            
            // Event that gets called whenever files have been opened (FileWatcher::CallbackInterface)
            void OnFilesOpened(vector<FileWatcher::File*> files);

            // Event that gets called whenever files have been closed (FileWatcher::CallbackInterface)
            void OnFilesClosed(vector<FileWatcher::File*> files);

            // An aggregate function that can either add or remove files (from the open file list, _openFileAbsolutePaths, _openFileRelativePaths)
            void AddOrRemove(vector<FileWatcher::File*> files, bool remove);

            // Process a completed file action and decide which function to call next (ProcessLockFileAction, etc...)
            void ProcessFileAction(Folder folder, FileAction fileAction, bool verbose);
            
            // React to a lock file action, display additional dialog windows, if necessary
            void ProcessLockFileAction(Folder folder, FileAction fileAction, bool verbose);
            
            // React to an unlock file action, display additional dialog windows, if necessary
            void ProcessUnlockFileAction(Folder folder, FileAction fileAction, bool verbose);
            
            // React to request unlock file action, display additional windows, if necessary
            void ProcessRequestUnlockFileAction(Folder folder, FileAction fileAction, bool verbose);
    };
}
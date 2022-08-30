#pragma once

#ifndef UNICODE
#define UNICODE 1
#endif

#include <string>
#include <unordered_set>
#include "folder.h"
#include "file_action.h"
#include "unlock_message.h"
#include "request.h"

#ifdef FFM_API_IMPL
#include "json.hpp"
using json = nlohmann::json;
#endif

using namespace std;

namespace FilesFM {
    class Api {
        private:
            #ifdef FFM_API_IMPL
            #endif

        public:
            // Wether the user associated with the folder should use file locking
            static bool GetUseFileLocking(Folder folder);

            // Lock the specified file
            static FileAction LockFile(Folder folder, wstring filePath);

            // Unlock the specified file
            static FileAction UnlockFile(Folder folder, wstring filePath);

            // Get unlock messages (requests and confirmations), if there are any
            static list<UnlockMessage> GetUnlockMessages(Folder folder);

            // Make the get_unlock_messages request
            static Request GetUnlockMessagesRequest(Folder folder);

            // Process unlock messages and act accordingly
            static list<UnlockMessage> ProcessUnlockMessagesRequest(Request request);

            // Crate multiple file actions and execute them
            static list<FileAction> CreateAndExecuteMultipleFileActions(Folder folder, wstring action, unordered_set<wstring> relativePaths);

            // Create a file action and execute it right after
            static FileAction CreateAndExecuteFileAction(Folder folder, wstring action, wstring relativePath);

            // Create multiple file actions
            static list<FileAction> CreateMultipleFileActions(Folder folder, wstring action, unordered_set<wstring> relativePaths);

            // Create a file action
            static FileAction CreateFileAction(Folder folder, wstring action, wstring relativePath);

            // Execute a file action
            static FileAction ExecuteFileAction(FileAction fileAction);

            // Execute multiple file actions (all fileActions MUST be based on the same folder)
            static list<FileAction> ExecuteMultipleFileActions(list<FileAction> fileActions);
    };
}
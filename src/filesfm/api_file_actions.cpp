#ifndef UNICODE
#define UNICODE 1
#endif

#define FFM_API_IMPL
#include "api.h"

#include "request.h"
#include "helpers.h"
#include "debug.h"
#include "logger.h"

#include "json.hpp"
using json = nlohmann::json;

using namespace std;

namespace FilesFM {

list<FileAction> Api::CreateAndExecuteMultipleFileActions(Folder folder, wstring action, unordered_set<wstring> relativePaths)
{
    auto fileActions = CreateMultipleFileActions(folder, action, relativePaths);
    fileActions = ExecuteMultipleFileActions(fileActions);

    return fileActions;
}


FileAction Api::CreateAndExecuteFileAction(Folder folder, wstring action, wstring relativePath)
{
    auto fileAction = CreateFileAction(folder, action, relativePath);
    fileAction = ExecuteFileAction(fileAction);

    return fileAction;
}

list<FileAction> Api::CreateMultipleFileActions(Folder folder, wstring action, unordered_set<wstring> relativePaths)
{
    list<FileAction> fileActions;

    for (auto relativePath : relativePaths) {
        auto fileAction = CreateFileAction(folder, action, relativePath);
        fileActions.push_back(fileAction);
    }

    return fileActions;
}

FileAction Api::CreateFileAction(Folder folder, wstring action, wstring relativePath)
{
    FileAction fileAction;
    fileAction.Action = action;
    fileAction.Path = relativePath;
    fileAction.Folder = folder;

    return fileAction;
}

FileAction Api::ExecuteFileAction(FileAction fileAction)
{
    auto fileActions = ExecuteMultipleFileActions({ fileAction });
    return fileActions.front();
}

list<FileAction> Api::ExecuteMultipleFileActions(list<FileAction> fileActions)
{
    if (fileActions.size() == 0) {
        throw std::invalid_argument("Api::ExecuteMultipleFileActions, fileActions.size() cannot be 0");
    }

    FileAction firstFileAction = fileActions.front();
    Request request(firstFileAction.Action, firstFileAction.Folder.Username, firstFileAction.Folder.Password);

    string buffer;
    json j;
    json jFiles;

    // Populate request and reset each fileAction
    for (auto fileActionsIterator = fileActions.begin(); fileActionsIterator != fileActions.end(); fileActionsIterator++) {
        // Reset fileAction
        fileActionsIterator->Error = false;
        fileActionsIterator->Success = false;
        fileActionsIterator->LockedByOtherUser = false;
        fileActionsIterator->LockedByUsername = L"";

        // Add request parameter
        request.AddParameter(L"file_paths[]", fileActionsIterator->Path);
    }

    // Execute request
    if (!request.Execute(&buffer)) {
        goto error;
    }

    // Parse respomse
    try {
        j = json::parse(buffer);
    } catch (const std::exception &ex) {
        D(Logger::Out() << "Api::ExecuteMultipleFileActions, error while parsing JSON: " << ex.what() << endl);
        goto error;
    }

    // Get response file list
    try {
        jFiles = j["files"].get<list<json>>();
    } catch (const std::exception &ex) {
        D(Logger::Out() << "Api::ExecuteMultipleFileActions, error while getting file list: " << ex.what() << endl);
        goto error;
    }

    // Loop through responses
    {
        auto fileActionsIterator = fileActions.begin();
        auto jFilesIterator = jFiles.begin();

        while (fileActionsIterator != fileActions.end() && jFilesIterator != jFiles.end()) {
            auto jFile = *jFilesIterator;
            //auto fileAction = *fileActionsIterator;

            bool success;
            try {
                success = jFile["success"].get<bool>();
            } catch (const std::exception &ex) {
                D(Logger::Out() << "Api::ExecuteMultipleFileActions, error while getting file action result's status: " << ex.what() << endl);
                fileActionsIterator->Error = true;
                goto end_of_loop;
            }

            fileActionsIterator->Success = success;
            D(Logger::Out() << "Api::ExecuteMultipleFileActions, action: " << fileActionsIterator->Path << ", success: " << fileActionsIterator->Success << endl);

            // This is a bit of a hack since this is not generic functionality
            // It will not crash other requests, but it is specific to file locking
            if (!fileActionsIterator->Success) {
                try {
                    string username = jFile["file_lock_data"]["user"]["username"].get<string>();
                    fileActionsIterator->LockedByUsername = Helpers::ToWide(username);
                    fileActionsIterator->LockedByOtherUser = true;
                } catch (const std::exception &ex) {
                    // Do nothing
                }
            }

        end_of_loop:
            fileActionsIterator++;
            jFilesIterator++;
        }
    }

    for (auto fileAction : fileActions) {
        D(Logger::Out() << "Api::ExecuteMultipleFileActions, fileAction: " << fileAction.Path << ", success: " << fileAction.Success << endl);
    }

    return fileActions;

error:
    for (auto fileAction : fileActions) {
        fileAction.Error = true;
    }
}
}
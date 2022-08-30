#include "files.h"
#include "filesfm.h"
#include "helpers.h"
#include "api.h"
#include "debug.h"

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <future>
#include <thread>

using namespace std;

namespace FilesFM {

Files::Files(FilesFM *parent)
{
    filesFM = parent;
    filesFM->FileWatcher.CallbackClass = this;
}

Files::~Files()
{
}

void Files::AddOrRemove(vector<FileWatcher::File *> files, bool remove)
{
    D(Logger::Out() << L"Files::AddOrRemove, " << (remove ? L"remove" : L"add"));

    for (auto f : files) {
        D(Logger::Out() << L", '" << f->Path << "'");
    }

    D(Logger::Out() << endl);

    bool add = !remove;
    unordered_map<wstring, Folder *> modifiedFolders;
    unordered_map<wstring, unordered_set<wstring>> modifiedFilePaths;


    for (auto f : files) {
        auto folder = filesFM->Folders.GetByFilePath(f->Path);

        if (folder == 0) {
            D(Logger::Out() << L"ERROR: Files::AddOrRemove: folder == 0 > f->Path: " << f->Path << endl);
            return;
        }

        wstring relativeFilePath = Helpers::GetRelativePath(*folder, f->Path);
        wstring relativeRemoteFilePath = Helpers::GetRelativeRemotePath(*folder, f->Path);

        //
        // Update the folders' open file list
        //
        auto it = folder->openFileRelativePaths.find(relativeFilePath);
        auto end = folder->openFileRelativePaths.end();

        if (add && it == end) {
            folder->openFileRelativePaths.insert(relativeFilePath);
        }

        if (remove && it != end) {
            folder->openFileRelativePaths.erase(it);
        }


        //
        // Update modified folders list
        //
        if (modifiedFolders.find(folder->Path) == modifiedFolders.end()) {
            modifiedFolders.insert({ folder->Path, folder });
        }

        // Update modified file paths (grouped by folder path)
        auto mfpIt = modifiedFilePaths.find(folder->Path);

        if (mfpIt == modifiedFilePaths.end()) {
            unordered_set<wstring> _files;
            modifiedFilePaths.insert({ folder->Path, _files });

            mfpIt = modifiedFilePaths.find(folder->Path);
        }

        mfpIt->second.insert(relativeRemoteFilePath);
    }

    D(Logger::Out() << L"Files::AddOrRemove, before folder loop" << endl);

    for (auto kvp : modifiedFolders) {
        int numRetries = 0;
        int maxNumRetries = 60;
        int retryIntervalMs = 1000;

        auto modifiedFolder = *(kvp.second);
        wstring action = add ? L"lock_file" : L"unlock_file";
        auto modifiedRelativePaths = modifiedFilePaths.find(modifiedFolder.Path)->second;

        D(Logger::Out() << L"Files::AddOrRemove, maxNumRetries: " << maxNumRetries << ", folder loop, modifiedRelativePaths: ");
        for (auto mRP : modifiedRelativePaths) {
            D(Logger::Out() << L", '" << mRP << "'");
        }
        D(Logger::Out() << endl);

        D(Logger::Out() << L"Files::AddOrRemove, folder loop, before CreateMultipleFileActions()" << endl);

        list<FileAction> fileActions;

        fileActions = Api::CreateMultipleFileActions(modifiedFolder, action, modifiedRelativePaths);
        
        D(Logger::Out() << L"Files::AddOrRemove, folder loop, after CreateMultipleFileActions()" << endl);

        while (numRetries < maxNumRetries) {
            numRetries++;

            fileActions = Api::ExecuteMultipleFileActions(fileActions);

            auto fileActionsIterator = fileActions.begin();
            while (fileActionsIterator != fileActions.end())
            {
                auto fileAction = *fileActionsIterator;
                auto lastFileActionsIterator = fileActionsIterator;
                fileActionsIterator++;

                if (fileAction.Success) {
                    D(Logger::Out() << L"Files::AddOrRemove, folder loop, removing fileAction: " << fileAction.Path << endl);

                    fileActions.erase(lastFileActionsIterator);
                    ProcessFileAction(modifiedFolder, fileAction, false);
                } else {
                    D(Logger::Out() << L"Files::AddOrRemove, folder loop, failed fileAction: " << fileAction.Path << endl);
                }
            }

            if (fileActions.size() == 0) {
                D(Logger::Out() << L"Files::AddOrRemove, breaking loop, numRetries: " << numRetries << endl);
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(retryIntervalMs));
        }

        if (fileActions.size() > 0) {
            for (auto fa : fileActions) {
                ProcessFileAction(modifiedFolder, fa, false);
            }
        }

        D(Logger::Out() << L"Files::AddOrRemove, folder loop done, fileActions.size(): " << fileActions.size() << endl);
    }

    D(Logger::Out() << L"Files::AddOrRemove, done" << endl);
}

void Files::ProcessFileAction(Folder folder, FileAction fileAction, bool verbose)
{
    D(Logger::Out() << L"Files::ProcessFileAction, folder.Path: " << folder.Path << ", fileAction.Path: " << fileAction.Path << endl);

    if (fileAction.Action.compare(L"lock_file") == 0) {
        ProcessLockFileAction(folder, fileAction, verbose);
    }

    if (fileAction.Action.compare(L"unlock_file") == 0) {
        ProcessUnlockFileAction(folder, fileAction, verbose);
    }

    if (fileAction.Action.compare(L"request_unlock_file_to_user") == 0) {
        ProcessRequestUnlockFileAction(folder, fileAction, verbose);
    }
}

void Files::ProcessLockFileAction(Folder folder, FileAction fileAction, bool verbose)
{
    if (fileAction.Success && verbose) {
        wstring msg = L"The file '" + fileAction.Path + L"' has been successfully locked!";

        MessageBox(
            NULL,
            msg.c_str(),
            (LPCWSTR)L"File locked",
            MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }

    if (!fileAction.Success) {
        if (fileAction.LockedByOtherUser) {
            wstring msg;
            msg += L"The file '" + fileAction.Path + L"' is locked by " + fileAction.LockedByUsername + L".\n";
            msg += L"If you will edit locked file, conflicting file versions may be created.\n";
            msg += L"Would you like to send an unlock request to the other user?";

            auto result = MessageBox(
                NULL,
                msg.c_str(),
                (LPCWSTR)L"File locked by another user",
                MB_OKCANCEL | MB_SYSTEMMODAL | MB_SETFOREGROUND);

            if (result == IDOK) {
                auto apiFileAction = Api::CreateAndExecuteFileAction(folder, L"request_unlock_file_to_user", fileAction.Path);
                ProcessFileAction(folder, apiFileAction, true);
            }
        } else {
            wstring msg = L"Could not lock file '" + fileAction.Path + L"', there was an error.";

            MessageBox(
                NULL,
                msg.c_str(),
                (LPCWSTR)L"File not locked",
                MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL | MB_SETFOREGROUND);
        }
    }
}


void Files::ProcessUnlockFileAction(Folder folder, FileAction fileAction, bool verbose)
{
    if (fileAction.Success && verbose) {
        wstring msg = L"The file '" + fileAction.Path + L"' has been successfully unlocked!";

        MessageBox(
            NULL,
            msg.c_str(),
            (LPCWSTR)L"File unlocked",
            MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }

    if (!fileAction.Success && verbose) {
        wstring msg = L"Could not unlock file '" + fileAction.Path + L"', there was an error.";

        MessageBox(
            NULL,
            msg.c_str(),
            (LPCWSTR)L"File not unlocked",
            MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }
}

void Files::ProcessRequestUnlockFileAction(Folder folder, FileAction fileAction, bool verbose)
{
    wstring title = L"File unlock request";

    if (fileAction.Success && verbose) {
        wstring msg = L"The file unlock request for the file '" + fileAction.Path + L"' has been successfully sent!";

        MessageBox(
            NULL,
            msg.c_str(),
            title.c_str(),
            MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }

    if (!fileAction.Success && verbose) {
        wstring msg = L"Could not send file unlock request for the file '" + fileAction.Path + L"', there was an error!";

        MessageBox(
            NULL,
            msg.c_str(),
            title.c_str(),
            MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }
}

void Files::OnFilesOpened(vector<FileWatcher::File *> files)
{
    auto thread = new std::thread([this, files] {
        AddOrRemove(files, false);
    });
}

void Files::OnFilesClosed(vector<FileWatcher::File *> files)
{
    //auto thread = new std::thread([this, files] {
        AddOrRemove(files, true);
    //});
}

void Files::LockOrUnlockFile(wstring absolutePath, bool lock)
{
    bool unlock = !lock;

    auto folder = filesFM->Folders.GetByFilePath(absolutePath);

    if (folder == 0) {
        D(Logger::Out() << L"ERROR: Files::LockOrUnlockFile: folder == 0 > absolutePath: " << absolutePath << endl);
        return;
    }


    wstring relativeRemotePath = Helpers::GetRelativeRemotePath(*folder, absolutePath);

    if (lock) {
        auto res = Api::LockFile(*folder, relativeRemotePath);
        ProcessFileAction(*folder, res, true);
    }

    if (unlock) {
        auto res = Api::UnlockFile(*folder, relativeRemotePath);
        ProcessFileAction(*folder, res, true);
    }
}

void Files::LockFile(wstring absolutePath)
{
    LockOrUnlockFile(absolutePath, true);
}

void Files::UnlockFile(wstring absolutePath)
{
    LockOrUnlockFile(absolutePath, false);
}
}
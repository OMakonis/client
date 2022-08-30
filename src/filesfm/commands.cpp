#include "commands.h"

#include <Windows.h>
#include <iostream>

#include "filesfm.h"
#include "helpers.h"
#include "api.h"

#include "debug.h"


using namespace std;

namespace FilesFM {
    Commands::Commands(FilesFM* parent)
    {
        filesFM = parent;
    }

    Commands::~Commands(){}

    void Commands::OpenBrowserFile(wstring localFilePath)
    {
        _openBrowser(localFilePath, L"open");
    }

    void Commands::OpenBrowserShare(wstring localFilePath)
    {
        _openBrowser(localFilePath, L"share");
    }

    void Commands::OpenBrowserSendMessage(wstring localFilePath)
    {
        _openBrowser(localFilePath, L"send_message");
    }

    void Commands::OpenBrowserFileVersions(wstring localFilePath)
    {
        _openBrowser(localFilePath, L"view_versions");
    }

    void Commands::LockFile(wstring localFilePath)
    {
        filesFM->Files.LockFile(localFilePath);
    }

    void Commands::UnlockFile(wstring localFilePath)
    {
        filesFM->Files.UnlockFile(localFilePath);
    }

    void Commands::_openBrowser(wstring localFilePath, wstring action)
    {
        /*wstring vfsSuffix = L".filesfm";
        auto containsVfsSuffix = localFilePath.substr(localFilePath.size() - vfsSuffix.size()).compare(vfsSuffix) == 0;

        if ( containsVfsSuffix )
        {
            localFilePath = localFilePath.substr(0, localFilePath.size() - vfsSuffix.size());
        }*/

        auto folder = filesFM->Folders.GetByFilePath(localFilePath);
        if ( folder == 0 )
        {
            D(Logger::Out() << L"ERROR: Commands::_openBrowser: folder == 0, localFilePath: " << localFilePath << L", action: " << action << endl);
            return;
        }

        wstring relativePath = Helpers::GetRelativeRemotePath(*folder, localFilePath);

        Helpers::OpenBrowser(action, folder->Username, relativePath);
    }
}
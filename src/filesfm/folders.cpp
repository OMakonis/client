#include "folders.h"

#include <iostream>

#include "filesfm.h"
#include "helpers.h"
#include "api.h"

#include "debug.h"

using namespace std;

namespace FilesFM {
    Folders::Folders(FilesFM* parent)
    {
        filesFM = parent;
    }

    Folders::~Folders()
    {
        
    }

    void Folders::Update(list<Folder> inFolders)
    {
        // Update folders
        for ( auto inFolder : inFolders )
        {
            if ( List.find(inFolder.Path) == List.end() )
            {
                Folder folder;
                folder.Username = inFolder.Username;
                folder.Password = inFolder.Password;
                folder.Path = inFolder.Path;
                folder.RemotePath = inFolder.RemotePath;

                D(Logger::Out() << "Adding folder '" << folder.Path << "'" << endl);
                List.insert({folder.Path, folder});
            }

            UpdateStatus(inFolder);
        }

        // Remove folders
        auto foldersIterator = List.begin();
        while(foldersIterator != List.end())
        {
            bool found = false;
            auto folder = foldersIterator->second;

            for ( auto inFolder : inFolders )
            {
                if ( inFolder.Path.compare(folder.Path) == 0 )
                {
                    found = true;
                    break;
                }
            }

            if ( !found )
            {
                D(Logger::Out() << "Removing folder, path: '" << folder.Path << "'" << endl);
                filesFM->FileWatcher.RemovePathToMonitor(folder.Path);
                foldersIterator = List.erase(foldersIterator);
            }

            foldersIterator++;
        }

        // Start/stop FileWatcher
        bool runFileWatcher = false;
        for ( auto f : List )
        {
            if ( f.second.UseFileLocking )
            {
                runFileWatcher = true;
                break;
            }
        }

        if ( runFileWatcher )
        {
            if ( !filesFM->FileWatcher.Running )
            {
                filesFM->FileWatcher.Start();
            }
        }
        else
        {
            if ( filesFM->FileWatcher.Running )
            {
                filesFM->FileWatcher.Stop();
            }
        }
    }

    void Folders::UpdateStatus(Folder inFolder)
    {
        auto folder = List[inFolder.Path];
        bool previousCanSyncState = folder.CanSync;
        folder.Password = inFolder.Password;
        folder.CanSync = inFolder.CanSync;

        if ( folder.CanSync && !previousCanSyncState )
        {
            if ( !folder.UseFileLocking )
            {
                folder.UseFileLocking = Api::GetUseFileLocking(folder);
            }

            if ( folder.UseFileLocking )
            {
                filesFM->FileWatcher.AddPathToMonitor(folder.Path);
            }
        }

        if ( !folder.CanSync && folder.UseFileLocking && previousCanSyncState )
        {
            filesFM->FileWatcher.RemovePathToMonitor(folder.Path);
        }

        List[folder.Path] = folder;
    }

    Folder* Folders::GetByFilePath(wstring filePath)
    {
        Helpers::FixPath(filePath);
        
        auto it = List.begin();

        while ( it != List.end() )
        {
            if ( Helpers::PathContainsPath(filePath, it->second.Path) )
            {
                return &it->second;
            }

            it++;
        }

        D(Logger::Out() << L"ERROR: Folders::GetByFilePath: NOT FOUND, filePath: " << filePath << endl);

        return 0;
    }

    bool Folders::GetUseFileLocking(wstring absolutePath)
    {
        auto folder = GetByFilePath(absolutePath);

        if ( folder == 0 )
        {
            D(Logger::Out() << L"ERROR: Folders::GetUseFileLocking: folder == 0, absolutePath: " << absolutePath << endl);
            return false;
        }

        return folder->UseFileLocking;
    }
}
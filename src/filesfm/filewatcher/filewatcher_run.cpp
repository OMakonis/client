#include "filewatcher.h"

#include <iostream>

#include "helpers.h"
#include "../helpers.h"

using namespace std;

namespace FilesFM {
    namespace FileWatcher {

        void FileWatcher::Run()
        {
            if ( Terminate )
            {
                return;
            }

            auto currentProcessId = GetCurrentProcessId();

            SYSTEM_HANDLE_ENTRY sh;
            
            Process* currentProcess = NULL;
            vector<File*>::iterator currentProcessFilesIterator;
            

            auto sysHandles = Helpers::GetSystemHandles();
            auto processIterator = _processes.begin();
            
            unordered_set<DWORD> runningProcessIds;

            list<Process*> startedProcesses;
            list<Process*> stoppedProcesses;       
            
            vector<File*> openedFiles;
            vector<File*> closedFiles;

            int throttleIntervalCounter = 0;

            int numHandlesProcessed = 0;

            unordered_set<BYTE> invalidObjectTypes;

            //
            // Loop through all returned SYSTEM_HANDLE objects
            // The returned data is a flat list with processes in random order, the process HANDLE lists are ordered, though!
            //
            for (DWORD i = 0; i < sysHandles->Count; i++)
            {
                if ( Terminate )
                {
                    break;
                }

                sh = sysHandles->Handle[i];

                // skip handles that have been proven to be of wrong type, if the right type isn't known yet
                if (_fileObjectType == 0 && invalidObjectTypes.find(sh.ObjectType) != invalidObjectTypes.end())
                {
                    continue;
                }

                // Check if this is a 'file' type of object (sadly that also includes pipes)
                // The type id might be different on different OS versions, needs testing
                if (_fileObjectType != 0 && sh.ObjectType != _fileObjectType)
                {
                    continue;
                }



                //
                // Check if the current process hasn't been set or the current sh points to a different process ID
                //
                if (currentProcess == NULL || currentProcess->Id != sh.OwnerPid)
                {
                    // If there already was a currentProcess, then add any unprocessed handles to the oldHandles list.
                    if (currentProcess != NULL)
                    {
                        while (currentProcessFilesIterator != currentProcess->Files.end())
                        {
                            closedFiles.push_back(*currentProcessFilesIterator);
                            currentProcessFilesIterator = currentProcess->Files.erase(currentProcessFilesIterator);
                        }
                    }

                    // Try to find the process in process list by its ID
                    auto foundProcess = _processes.find(sh.OwnerPid);

                    // This is a new process
                    if (foundProcess == _processes.end())
                    {
                        currentProcess = Helpers::OpenAndPrepareProcess(sh.OwnerPid);
                        currentProcess->Valid &= _processNamesToSkip.find(currentProcess->Name) == _processNamesToSkip.end();
                        currentProcess->Valid &= !FilesFM::Helpers::PathContainsPaths(currentProcess->Path, &_pathsToSkip);

                        startedProcesses.push_back(currentProcess);

                        _addProcess(currentProcess);
                    }
                    // Found process info
                    else
                    {
                        currentProcess = foundProcess->second;
                    }

                    currentProcessFilesIterator = currentProcess->Files.begin();
                    runningProcessIds.insert(currentProcess->Id);
                }



                //
                // Throttling (CPU usage optimization)
                //
                if (!_isFirstRun)
                {
                    if (UseThrottling && throttleIntervalCounter >= ThrottleInterval)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(ThrottleDelay));
                        throttleIntervalCounter = 0;
                    }
                    else
                    {
                        throttleIntervalCounter++;
                    }
                }



                //
                // Skip current handle (if the current process is invalid)
                //
                if (!currentProcess->Valid)
                {
                    continue;
                }



                //
                // Check the handle against the process' handles list
                //
                bool isNewHandle = false;
                

                if (currentProcessFilesIterator == currentProcess->Files.end())
                {
                    isNewHandle = true;
                }
                else
                {
                    while (currentProcessFilesIterator != currentProcess->Files.end())
                    {
                        File* processFile = *currentProcessFilesIterator;

                        // Closed handle
                        if (processFile->Handle < (HANDLE)sh.HandleValue)
                        {
                            closedFiles.push_back(processFile);
                            currentProcessFilesIterator = currentProcess->Files.erase(currentProcessFilesIterator);
                        }
                        // Existing handle, do nothing
                        else if (processFile->Handle == (HANDLE)sh.HandleValue)
                        {
                            currentProcessFilesIterator++;
                            break;
                        }
                        // New handle
                        else if (processFile->Handle > (HANDLE)sh.HandleValue)
                        {
                            isNewHandle = true;
                            break;
                        }
                    }
                }



                //
                // Process the new handle
                //
                if (isNewHandle)
                {
                    HANDLE dupedHandle;
                    File* File = new FilesFM::FileWatcher::File;
                    File->Handle = (HANDLE)sh.HandleValue;
                    File->Process = (PVOID)currentProcess;

                    if (DuplicateHandle(currentProcess->Handle, File->Handle, GetCurrentProcess(), &dupedHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
                    {
                        auto fileType = GetFileType(dupedHandle);

                        if (fileType == FILE_TYPE_DISK)
                        {
                            if (_fileObjectType == 0)
                            {
                                _fileObjectType = sh.ObjectType;
                            }

                            BY_HANDLE_FILE_INFORMATION Filermation;
                            auto gotFilermation = GetFileInformationByHandle(dupedHandle, &Filermation);
                            
                            wstring path;
                            auto gotPath = Helpers::GetFilePathFromHandle(dupedHandle, path);

                            if (gotFilermation && gotPath && path.size() > 0 && Filermation.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
                            {					
                                File->Path = path;
                                File->Name = FilesFM::Helpers::GetFileNameFromPath(path);
                                
                                // Exclude all files, that start with a '.'
                                if ( File->Name.substr(0,1).compare(L".") == 0)
                                {
                                    continue;
                                }

                                File->Valid = !FilesFM::Helpers::PathContainsPaths(File->Path, &_pathsToSkip);

                                if ( File->Valid )
                                {
                                    bool folderFound = false;

                                    for ( auto pathToMonitor : _pathsToMonitor )
                                    {
                                        if ( FilesFM::Helpers::PathContainsPath(File->Path, pathToMonitor) )
                                        {
                                            folderFound = true;
                                            break;
                                        }
                                    }

                                    if ( !folderFound )
                                    {
                                        File->Valid = false;
                                    }
                                }

                                if ( File->Valid )
                                {
                                    auto matchesExcludePatterns = FilesFM::Helpers::PathMatchesExcludePatterns(File->Name, _excludePatterns);
                                    File->Valid &= !matchesExcludePatterns;
                                }
                            }
                        }
                        else 
                        {
                            if (_fileObjectType == 0)
                            {
                                invalidObjectTypes.insert(sh.ObjectType);
                            }
                        }

                        CloseHandle(dupedHandle);
                    }

                    currentProcessFilesIterator = currentProcess->Files.insert(currentProcessFilesIterator, File);
                    currentProcessFilesIterator++;

                    openedFiles.push_back(File);
                }

            } // END of SYSTEM_HANDLE loop

            if ( Terminate )
            {
                goto cleanup;
            }



            //
            // Find stopped processes
            //
            processIterator = _processes.begin();

            while ( processIterator != _processes.end() )
            {
                if (runningProcessIds.find(processIterator->second->Id) == runningProcessIds.end())
                {
                    stoppedProcesses.push_back(processIterator->second);
                }

                processIterator++;
            }



            //
            // Add files from stopped processes to closed file list
            //
            for (auto p : stoppedProcesses)
            {
                auto fileIterator = p->Files.begin();

                while ( fileIterator != p->Files.end() )
                {
                    closedFiles.push_back(*fileIterator);
                    fileIterator = p->Files.erase(fileIterator);
                }
            }


            //
            // Handles opened files
            //
            if ( openedFiles.size() > 0 )
            {
                bool filesAdded = false;
                string openedFilesString = "";

                for (auto f : openedFiles)
                {
                    if (!f->Valid)
                    {
                        continue;
                    }

                    // wcout << "File opened: " << f->Path << endl;

                    OpenFiles.push_back(f);

                    filesAdded = true;
                }
            }

            for (auto f : closedFiles)
            {
                if (!f->Valid)
                {
                    continue;
                }

                // wcout << "File closed: " << f->Path << endl;

                auto openFileIterator = OpenFiles.begin();

                while ( openFileIterator != OpenFiles.end() )
                {
                    // Double pointer, be careful here
                    if ( (*openFileIterator)->Handle == f->Handle )
                    {
                        openFileIterator = OpenFiles.erase(openFileIterator);
                    }else{
                        openFileIterator++;
                    }
                }
            }
           
            if ( CallbackClass != 0 && closedFiles.size() > 0 )
            {
                vector<File*> validClosedFiles;

                for ( auto f : closedFiles )
                {
                    if ( f->Valid )
                    {
                        validClosedFiles.push_back(f);
                    }
                }
                
                if ( validClosedFiles.size() > 0 )
                {
                    CallbackClass->OnFilesClosed(validClosedFiles);
                }
            }

            if ( CallbackClass != 0 && openedFiles.size() > 0 )
            {
                vector<File*> validOpenedFiles;

                for ( auto f : openedFiles )
                {
                    if ( f->Valid )
                    {
                        validOpenedFiles.push_back(f);
                    }
                }
                
                if ( validOpenedFiles.size() > 0 )
                {
                    CallbackClass->OnFilesOpened(validOpenedFiles);
                }
            }


            //
            // Free up closedFiles File memory
            //
            for (auto f : closedFiles)
            {
                delete f;
            }



            //
            // Remove stopped processes
            //
            for (auto p : stoppedProcesses) 
            {
                _removeProcess(p);
            }


            cleanup:

            // Delete sysHandles to prevent a memory leak
            delete sysHandles;

            if ( _isFirstRun )
            {
                // Set _isFirstRun to false...
                _isFirstRun = false;
            }
        }
    }
}
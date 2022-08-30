#include "filewatcher.h"

#include "../helpers.h"
#include "../debug.h"
#include "../logger.h"

#include <iostream>

using namespace std;

namespace FilesFM {
    namespace FileWatcher {
        void FileWatcher::AddPathToMonitor(wstring path)
        {
            D(Logger::Out() << "FileWatcher::AddPathToMonitor, path: " << path << endl);

            auto pathIterator = _pathsToMonitor.find(path);
            if ( pathIterator == _pathsToMonitor.end() )
            {
                D(Logger::Out() << "FileWatcher::AddPathToMonitor, adding path: " << path << endl);
                _pathsToMonitor.insert(path);
            }
        }

        void FileWatcher::RemovePathToMonitor(wstring path)
        {
            D(Logger::Out() << "FileWatcher::RemovePathToMonitor, path: " << path << endl);

            auto pathIterator = _pathsToMonitor.find(path);
            if ( pathIterator != _pathsToMonitor.end() )
            {
                D(Logger::Out() << "FileWatcher::RemovePathToMonitor, removing path: " << path << endl);
                _pathsToMonitor.erase(pathIterator);
            }
        }

        void FileWatcher::AddProcessNameToSkip(string processName)
        {
            auto processNameWide = FilesFM::Helpers::ToWide(processName);
            AddProcessNameToSkip(processNameWide);
        }

        void FileWatcher::AddProcessNameToSkip(wstring processName)
        {
            D(Logger::Out() << "FileWatcher::AddProcessNameToSkip, processName: " << processName << endl);
            _processNamesToSkip.insert(processName);
        }

        void FileWatcher::AddPathToSkip(wstring path)
        {
            D(Logger::Out() << "FileWatcher::AddPathToSkip, path: " << path << endl);
            _pathsToSkip.insert(path);
        }

        void FileWatcher::_addProcess(Process* process)
        {
            _processes.insert({ process->Id, process });
        }


        void FileWatcher::_removeProcess(Process* process)
        {
            auto foundProcessIterator = _processes.find(process->Id);

            if (foundProcessIterator != _processes.end())
            {
                _removeProcess(foundProcessIterator);
            }
        }

        unordered_map<DWORD, Process*>::iterator FileWatcher::_removeProcess(unordered_map<DWORD, Process*>::iterator iterator)
        {
            //
            // Cleanup memory
            //
            for (auto f : iterator->second->Files)
            {
                delete f;
            }
            delete iterator->second;

            // Remove the element from the map
            return _processes.erase(iterator);

        }

        FileWatcher::FileWatcher()
        {

        }

        FileWatcher::~FileWatcher()
        {

        }

        void FileWatcher::Start()
        {
            if ( Running )
            {
                return;
            }

            D(Logger::Out() << "FileWatcher::Start" << endl);

            _mainThread = new std::thread([this]
            {
                Running = true;
            
                D(Logger::Out() << "FileWatcher, MainThread started" << endl);

                while(!Terminate)
                {
                    Run();
                }

                D(Logger::Out() << "FileWatcher, MainThread stopped" << endl);

                Running = false;
            });
        }

        void FileWatcher::Stop()
        {
            if ( !Running )
            {
                return;
            }

            D(Logger::Out() << "FileWatcher::Stop" << endl);

            Terminate = true;

            while(Running)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            Terminate = false;

            D(Logger::Out() << "FileWatcher, stopped" << endl);
        }
    }
}
#pragma once
#define UNICODE 1

#include <windows.h>

#include <string>
#include <list>
#include <vector>

#include "file.h"


using namespace std;

namespace FilesFM {
    namespace FileWatcher {

		class Process {
		public:
			DWORD Id = 0;
			HANDLE Handle = NULL;
			bool Valid = false;
			wstring Path;
			wstring Name;

			// Adds a file to the list of open files
			void AddFile(File* file);

			// Removes a file from the list of open files
			void RemoveFile(File* file);

			// Removes all provided files from the list of open files
			list<File*>::iterator RemoveFile(list<File*>::iterator iterator);

			// Holds all opened file infos for this process
			vector<File*> Files;

		};
		
	}
}

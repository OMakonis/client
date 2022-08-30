#pragma once
#include <windows.h>
#include <string>

using namespace std;

namespace FilesFM {
    namespace FileWatcher {

		class File
		{
		public:
			PVOID Process = NULL;
			bool Valid = false;
			DWORD ProcessId = 0;
			HANDLE Handle = NULL;
			wstring Path;
			wstring Name;
		};

	}
}

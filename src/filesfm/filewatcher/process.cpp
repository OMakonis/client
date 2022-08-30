#define UNICODE 1

#include "process.h"

namespace FilesFM {
    namespace FileWatcher {

		void Process::AddFile(File* file)
		{
			Files.push_back(file);
		}

		void Process::RemoveFile(File* file)
		{

		}

		list<File*>::iterator Process::RemoveFile(list<File*>::iterator iterator)
		{
			return list<File*>::iterator();
		}

	}
}

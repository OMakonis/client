#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

#include "filesfm.fwd.h"
#include "folder.h"

using namespace std;

namespace FilesFM {
    class Folders {
        private:

        public:
            FilesFM* filesFM;

            Folders(FilesFM* parent);
            ~Folders();

            unordered_map<wstring, Folder> List;
            mutex Mutex;

            Folder* GetByFilePath(wstring filePath);

            void Update(list<Folder> inFolders);

            void UpdateStatus(Folder inFolder);

            bool GetUseFileLocking(wstring absolutePath);
    };
}
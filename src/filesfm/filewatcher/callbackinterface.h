#pragma once

#include <vector>
#include "file.h"

using namespace std;

namespace FilesFM {
    namespace FileWatcher {
        __interface CallbackInterface {
                void OnFilesOpened(vector<File*> files);
                void OnFilesClosed(vector<File*> files);
        };
    }
}
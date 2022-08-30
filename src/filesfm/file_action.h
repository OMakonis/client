#pragma once

#ifndef UNICODE
#define UNICODE 1
#endif

#include <string>

namespace FilesFM {
    class FileAction {
        public:
        wstring Action;
        wstring Path;
        Folder Folder;
        bool Error = false;
        bool Success = false;
        bool LockedByOtherUser = false;
        wstring LockedByUsername;
    };
}
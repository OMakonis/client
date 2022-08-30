#pragma once

#ifndef UNICODE
#define UNICODE 1
#endif

#include <string>

using namespace std;

namespace FilesFM {
    class UnlockMessage {
        public:
        bool Request = false;
        bool Response = false;
        wstring Username;
        wstring FilePath;
        wstring FileName;
        int last_check_tmst = 0;
    };
}
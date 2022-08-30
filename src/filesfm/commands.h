#pragma once

#ifndef UNICODE
#define UNICODE 1
#endif

#include <string>

#include "filesfm.fwd.h"

using namespace std;

namespace FilesFM {
    class Commands {
        private:
        void _openBrowser(wstring localFilePath, wstring action);

        public:
        FilesFM* filesFM;

        Commands(FilesFM* parent);
        ~Commands();

        // Open the file in the browser
        void OpenBrowserFile(wstring localFilePath);

        // Open a file share dialog in the browser
        void OpenBrowserShare(wstring localFilePath);

        // Open a send message dialog in the browser
        void OpenBrowserSendMessage(wstring localFilePath);

        // Open previous file versions in the browser
        void OpenBrowserFileVersions(wstring localFilePath);

        // Send lock file command through the API
        void LockFile(wstring localFilePath);

        // Send unlock file command through the API
        void UnlockFile(wstring localFilePath);
    };
}
#include "logs.h"

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "filesfm.h"
#include "helpers.h"
#include "request.h"

#include <filesystem>


using namespace std;

namespace FilesFM {

    Logs::Logs(FilesFM* parent)
    {
        filesFM = parent;
    }

    Logs::~Logs()
    {

    }
    
    void Logs::Upload(list<wstring> folderPaths, bool sendNudesMode)
    {
        wstring logFileName = L".owncloudsync.log";

        new std::thread([this, folderPaths, logFileName, sendNudesMode]
        {
            int numFilesUploaded = 0;

            for ( auto fp : folderPaths )
            {
                wstringstream ws;
                ws << fp << logFileName;

                wstring logPath = ws.str();

                if ( Helpers::FileExists(logPath) )
                {
                    if ( UploadFile(logPath) )
                    {
                        numFilesUploaded++;
                    }
                }
            }

            if ( numFilesUploaded >= folderPaths.size() )
            {
                string title;
                string msg;

                if ( sendNudesMode )
                {
                    title = "Nudes transmission report";
                    msg = "All nudes have been sent!";
                }else{
                    title = "Log file upload";
                    msg = "All log files have been uploaded!";
                }

                MessageBoxA(
                    NULL,
                    msg.c_str(),
                    title.c_str(),
                    MB_OK
                );
            }
            else
            {
                wstringstream ws;
                ws << L"There were errors while uploading log files." << endl;
                ws << numFilesUploaded << L" of " << folderPaths.size() << L" files were uploaded." << endl;

                MessageBox(
                    NULL,
                    ws.str().c_str(),
                    L"Log file upload",
                    MB_OK
                );
            }
        });
    }

    bool Logs::UploadFile(wstring path)
    {
        auto folder = filesFM->Folders.GetByFilePath(path);

        if ( folder == 0 )
        {
            return false;
        }

        Request request(L"upload", folder->Username, folder->Password);
        request.Script = "log/upload";

        request.AddFile(L"log", path);

        string buffer;
        request.Execute(&buffer);

        if ( buffer.compare("OK") != 0 )
        {
            wstringstream ws;
            ws << "Failed to upload file for user: " << folder->Username;

            MessageBox(
                NULL,
                ws.str().c_str(),
                L"Log file upload",
                MB_OK
            );

            return false;
        }

        return true;
    }
}
#ifndef UNICODE
#define UNICODE 1
#endif

#include "helpers.h"

#include <windows.h>
#include <winternl.h>

#include "../helpers.h"

using namespace std;

namespace FilesFM {
    namespace FileWatcher {

        BOOL Helpers::GetFilePathFromHandle(HANDLE hFile, wstring& outPath)
        {
            TCHAR pathBuffer[MAX_PATH] = { L'\0' };
            auto length = GetFinalPathNameByHandle(hFile, pathBuffer, MAX_PATH, VOLUME_NAME_DOS | FILE_NAME_OPENED); // , FILE_NAME_OPENED& VOLUME_NAME_DOS);

            if (length > 4) // sometimes length can include \0, it seems
            {
                wstring path((wchar_t*)pathBuffer);

                path = path.substr(4, path.size() - 4);
                FilesFM::Helpers::FixPath(path);

                outPath = path;
            }

            return length > 0;
        }

        SYSTEM_HANDLE_INFORMATION* Helpers::GetSystemHandles()
        {
            SYSTEM_HANDLE_INFORMATION* sysHandleInformation;
            ULONG size = 0;
            ULONG needed = 0;
            NTSTATUS status;
            bool finished = false;

            do {
                size = needed + 1024;
                sysHandleInformation = (SYSTEM_HANDLE_INFORMATION*)new byte[size];

                status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0X10, sysHandleInformation, size, &needed);

                if (NT_SUCCESS(status))
                {
                    finished = true;
                }
                else
                {
                    delete[] sysHandleInformation;
                }
            } 
            while (!finished);

            return sysHandleInformation;
        }

        Process* Helpers::OpenAndPrepareProcess(DWORD processId)
        {
            Process* process = new Process;
            process->Id = processId;
            process->Files.reserve(512);

            auto rights = PROCESS_DUP_HANDLE | PROCESS_QUERY_LIMITED_INFORMATION;

            HANDLE hProcess = OpenProcess(rights, FALSE, processId);

            if (hProcess)
            {
                process->Handle = hProcess;
                TCHAR tcPath[2048] = { L'\0' };

                DWORD tcPathSize = sizeof(tcPath) / sizeof(*tcPath);

                if (QueryFullProcessImageName(hProcess, 0, tcPath, &tcPathSize))
                {
                    wstring path((wchar_t*)tcPath);

                    FilesFM::Helpers::FixPath(path);

                    process->Path = path;
                    process->Name = FilesFM::Helpers::GetFileNameFromPath(process->Path);
                }

                process->Valid = process->Path.length() > 0 && process->Name.length() > 0;
            }

            return process;
        }

    }
}

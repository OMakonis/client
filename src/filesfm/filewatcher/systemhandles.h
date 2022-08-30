#pragma once
#define UNICODE 1

#include <windows.h>

using namespace std;

namespace FilesFM {
    namespace FileWatcher {

        typedef struct _SYSTEM_HANDLE_ENTRY
        {
            ULONG 	OwnerPid;
            BYTE 	ObjectType;
            BYTE 	HandleFlags;
            USHORT 	HandleValue;
            PVOID 	ObjectPointer;
            ULONG 	AccessMask;
        } SYSTEM_HANDLE_ENTRY, *PSYSTEM_HANDLE_ENTRY;

        typedef struct _SYSTEM_HANDLE_INFORMATION
        {
            ULONG         Count;
            SYSTEM_HANDLE_ENTRY Handle[1];
        } SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION, ** PPSYSTEM_HANDLE_INFORMATION;
        
    }
}
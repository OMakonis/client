#ifndef UNICODE
#define UNICODE 1
#endif

#define FFM_API_IMPL
#include "api.h"

#include <windows.h>

#include <mutex>
#include <thread>
#include <iostream>
#include "request.h"
#include "helpers.h"
#include "debug.h"
#include "logger.h"

#include "json.hpp"
using json = nlohmann::json;

using namespace std;

namespace FilesFM {

std::map<std::wstring, bool> UseFileLockingCache;

bool Api::GetUseFileLocking(Folder folder)
{
    // Check cache and return the cached result
    if (UseFileLockingCache.count(folder.Username)) {
        Logger::Out() << "Api::GetUseFileLocking, returning cached value: " << UseFileLockingCache[folder.Username] << endl;
        return UseFileLockingCache[folder.Username];
    }

    D(Logger::Out() << "Api::GetUseFileLocking, folder: " << folder.Path << endl);

    string buffer;

    Request request(L"check_if_use_file_locking", folder.Username, folder.Password);
    if (!request.Execute(&buffer)) {
        return false;
    }

    json j;
    try {
        j = json::parse(buffer);
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUseFileLocking, error while trying to pare JSON: " << ex.what() << endl;
        return false;
    }

    bool success = false;
    try {
        success = j["success"].get<bool>();
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUseFileLocking, error while trying get status: " << ex.what() << endl;
        return false;
    }

    bool useFileLocking = false;
    try {
        useFileLocking = j["use_file_locking"].get<bool>();
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUseFileLocking, error while trying get use_file_locking state: " << ex.what() << endl;
        return false;
    }

    UseFileLockingCache[folder.Username] = useFileLocking;

    return useFileLocking;
}

FileAction Api::LockFile(Folder folder, wstring filePath)
{
    D(Logger::Out() << "Api::LockFile, folder: " << folder.Path << ", filePath: " << filePath << endl);

    auto res = CreateAndExecuteFileAction(folder, L"lock_file", filePath);
    return res;
}

FileAction Api::UnlockFile(Folder folder, wstring filePath)
{
    D(Logger::Out() << "Api::UnlockFile, folder: " << folder.Path << ", filePath: " << filePath << endl);

    auto res = CreateAndExecuteFileAction(folder, L"unlock_file", filePath);
    return res;
}

Request Api::GetUnlockMessagesRequest(Folder folder)
{
    Request request(L"get_unlock_messages", folder.Username, folder.Password);
    request.AddParameter(L"last_check_tmst", to_wstring(folder.last_check_tmst));

    return request;
}

list<UnlockMessage> Api::ProcessUnlockMessagesRequest(Request request)
{
    list<UnlockMessage> res;

    return res;
}

list<UnlockMessage> Api::GetUnlockMessages(Folder folder)
{
    // D(Logger::Out() << "Api::GetUnlockMessages, folder: " << folder.Path << endl);

    list<UnlockMessage> res;

    Request request(L"get_unlock_messages", folder.Username, folder.Password);
    request.AddParameter(L"last_check_tmst", to_wstring(folder.last_check_tmst));

    string buffer;
    if (!request.Execute(&buffer)) {
        return res;
    }

    json j;
    try {
        j = json::parse(buffer);
    } catch (const std::exception &ex) {
        D(Logger::Out() << "Api::GetUnlockMessages, error while trying to parse JSON: " << ex.what() << endl);
        return res;
    }


    bool success = false;
    try {
        success = j["success"].get<bool>();
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUnlockMessages, error while trying to get success status: " << ex.what() << endl;
        return res;
    }

    if (!success || j["check_tmst"] == NULL) {
        return res;
    }


    int check_tmst;
    try {
        check_tmst = j["check_tmst"].get<int>();
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUnlockMessages, error while trying to get check_tmst: " << ex.what() << endl;
        return res;
    }


    json unlockMessages;
    try {
        unlockMessages = j["unlock_messages"];
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUnlockMessages, error while trying to get unlock_messages: " << ex.what() << endl;
        return res;
    }


    //
    // Process requests
    //
    try {
        auto unlockRequests = unlockMessages["requests"];
        for (json::iterator it = unlockRequests.begin(); it != unlockRequests.end(); ++it) {
            json unlockRequest = *it;
            string username;
            string filename;
            string path;

            try {
                username = unlockRequest["user"]["username"].get<string>();
                filename = unlockRequest["file"]["name"].get<string>();
                path = unlockRequest["file"]["path"].get<string>();

                UnlockMessage um;
                um.Request = true;
                um.Username = Helpers::ToWide(username);
                um.FileName = Helpers::ToWide(filename);
                um.FilePath = Helpers::ToWide(path);
                um.last_check_tmst = check_tmst;

                res.push_back(um);
            } catch (const std::exception &ex) {
                Logger::Out() << "Api::GetUnlockMessages, error while trying to process unlock request: " << ex.what() << endl;
            }
        }
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUnlockMessages, error while trying to process unlockRequests: " << ex.what() << endl;
    }


    //
    // Process responses
    //
    try {
        auto unlockResponses = unlockMessages["responses"];
        for (json::iterator it = unlockResponses.begin(); it != unlockResponses.end(); ++it) {
            json unlockRequest = *it;
            string username;
            string filename;
            string path;

            try {
                username = unlockRequest["user"]["username"].get<string>();
                filename = unlockRequest["file"]["name"].get<string>();
                path = unlockRequest["file"]["path"].get<string>();

                UnlockMessage um;
                um.Response = true;
                um.Username = Helpers::ToWide(username);
                um.FileName = Helpers::ToWide(filename);
                um.FilePath = Helpers::ToWide(path);
                um.last_check_tmst = check_tmst;

                res.push_back(um);
            } catch (const std::exception &ex) {
                Logger::Out() << "Api::GetUnlockMessages, error while trying to process unlock request: " << ex.what() << endl;
            }
        }
    } catch (const std::exception &ex) {
        Logger::Out() << "Api::GetUnlockMessages, error while trying to process unlockRequests: " << ex.what() << endl;
    }

    return res;
}
}
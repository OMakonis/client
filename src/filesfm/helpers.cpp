#include "helpers.h"

#include <windows.h>
#include <ShellApi.h>

#include <algorithm>
#include <codecvt>
#include <locale>
#include <sstream>

using namespace std;

namespace FilesFM {
    string Helpers::ToNarrow(wstring input)
    {
        using convert_type = codecvt_utf8<wchar_t>;
        wstring_convert<convert_type, wchar_t> converter;

        string result = converter.to_bytes(input);
        return result;
    }

    wstring Helpers::ToWide(string input)
    {
        using convert_type = codecvt_utf8<wchar_t>;
        wstring_convert<convert_type, wchar_t> converter;

        return converter.from_bytes(input);
    }

    wstring Helpers::ToLower(wstring text)
    {
        transform(
            text.begin(), text.end(),
            text.begin(),
            towlower
        );

        return text;
    }

    wstring Helpers::FixWindowsPath(wstring path)
    {
        if (path.size() > 4)
        {
            auto start = path.begin() + 4;

            transform(
                start, path.end(),
                start,
                towlower
            );
        }

        return path;
    }

    // containingPath: C:\Users\Test\MyFile.txt
    // path: C:\Users\Test
    // result: true
    bool Helpers::PathContainsPath(wstring containingPath, wstring path)
    {
        if (containingPath.size() < path.size())
        {
            return false;
        }

        auto result = containingPath.substr(0, path.size()).compare(path) == 0;
        return result;
    }

    // containingPath: C:\Users\Test\MyFile.txt
    // paths: [C:\Windows, C:\Users\Test]
    // result: true
    bool Helpers::PathContainsPaths(wstring containingPath, unordered_set<wstring>* paths)
    {
        for (auto path : *paths)
        {
            if (PathContainsPath(containingPath, path))
            {
                return true;
            }
        }

        return false;
    }

    wstring Helpers::GetFileNameFromPath(wstring path)
    {
        wstring filename = L"";

        size_t found = path.find_last_of(L"/\\");
        if (found > 0)
        {
            filename = path.substr(found + 1);
        }

        return filename;
    }

    wstring Helpers::GetFolderPath(const KNOWNFOLDERID folderId)
    {
        PWSTR path = NULL;

        SHGetKnownFolderPath(folderId, 0, NULL, &path);
        wstring result(path);

        // This should be called, according to the docs
        // https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath?redirectedfrom=MSDN
        CoTaskMemFree(path);

        return result;
    }

    // Taken from: https://www.geeksforgeeks.org/wildcard-character-matching/
    bool Helpers::Match(const wchar_t *pattern, const wchar_t * tomatch) 
    { 
        // If we reach at the end of both strings, we are done 
        if (*pattern == '\0' && *tomatch == '\0') 
            return true; 
    
        // Make sure that the characters after '*' are present 
        // in second string. This function assumes that the first 
        // string will not contain two consecutive '*' 
        if (*pattern == '*' && *(pattern+1) != '\0' && *tomatch == '\0') 
            return false; 
    
        // If the first string contains '?', or current characters 
        // of both strings match 
        if (*pattern == '?' || *pattern == *tomatch) 
            return Match(pattern+1, tomatch+1); 
    
        // If there is *, then there are two possibilities 
        // a) We consider current character of second string 
        // b) We ignore current character of second string. 
        if (*pattern == '*') 
            return Match(pattern+1, tomatch) || Match(pattern, tomatch+1); 
        return false; 
    } 

    bool Helpers::PathMatchesExcludePatterns(wstring path, list<wstring> excludePatterns) 
    {
        for ( auto ep : excludePatterns )
        {
            if (Match(ep.c_str(), path.c_str()))
            {
                return true;
            }
        }

        return false;
    }

    void Helpers::FixPath(wstring & path)
    {
        FindAndReplaceAll(path, L"\\", L"/");
    }

    void Helpers::FindAndReplaceAll(string & data, string toSearch, string replaceStr)
    {
        // Get the first occurrence
        size_t pos = data.find(toSearch);
    
        // Repeat till end is reached
        while( pos != string::npos)
        {
            // Replace this occurrence of Sub String
            data.replace(pos, toSearch.size(), replaceStr);
            // Get the next occurrence from the current position
            pos =data.find(toSearch, pos + replaceStr.size());
        }
    }
 
    void Helpers::FindAndReplaceAll(wstring & data, wstring toSearch, wstring replaceStr)
    {
        // Get the first occurrence
        size_t pos = data.find(toSearch);
    
        // Repeat till end is reached
        while( pos != string::npos)
        {
            // Replace this occurrence of Sub String
            data.replace(pos, toSearch.size(), replaceStr);
            // Get the next occurrence from the current position
            pos =data.find(toSearch, pos + replaceStr.size());
        }
    }

    wstring Helpers::GetRelativeRemotePath(Folder folder, wstring filePath)
    {
        Helpers::FixPath(filePath);

        wstringstream ws;
        auto relativePath = GetRelativePath(folder, filePath);

        if ( folder.RemotePath.size() > 1 )
        {
            ws << folder.RemotePath;
        }

        ws << relativePath;

        return ws.str().c_str();
    }

    wstring Helpers::GetRelativePath(Folder folder, wstring filePath)
    {
        auto folderPathLength = folder.Path.length() - 1;
        auto filePathLength = filePath.length();

        if ( folderPathLength > filePathLength )
        {
            return L"";
        }

        wstring relativePath = filePath.substr(folderPathLength, filePathLength - folderPathLength);

        return relativePath;
    }

    void Helpers::OpenBrowser(wstring action, wstring username, wstring relativePath)
    {
        std::wstring url = L"https://files.fm/server_scripts/filesfm_sync_contextmenu_action.php?username=";
        url.append(username);
        url.append(L"&action=");
        url.append(action);
        url.append(L"&path=");
        url.append(relativePath);

        ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    bool Helpers::FileExists(wstring path)
    {
        struct stat buffer;  
        return (stat (ToNarrow(path).c_str(), &buffer) == 0); 
    }
}
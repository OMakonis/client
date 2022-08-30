#pragma once

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#include <list>
#include <string>
#include <unordered_set>
#include <ShlObj.h>

#include "folder.h"

using namespace std;

namespace FilesFM {
    class Helpers {
        public:

        // Converts to lowercase
        static wstring ToLower(wstring source);

        // Returns the windows path so it looks like this 'C:\Windows'
        static wstring FixWindowsPath(wstring path);	

        // Converts std::wstring to std::string (from wide to narrow)
        static string ToNarrow(wstring input);

        // Converts std::string to std::wstring (from narrow to wide)
        static wstring ToWide(string input);

        // Returns if needle is contained within containingPath
        static bool PathContainsPath(wstring containingPath, wstring path);

        // Returns true if needle is contained within one of the passed paths
        static bool PathContainsPaths(wstring containingPath, unordered_set<wstring>* paths);

        // Returns the filename, or the last part of path
        static wstring GetFileNameFromPath(wstring path);

        // Returns a system folder path (like 'My Documents')
        static wstring GetFolderPath(const KNOWNFOLDERID folderId);

        // Returns true if the path matches against any of the provided excludePatterns
        static bool PathMatchesExcludePatterns(wstring path, list<wstring> excludePatterns);

        // Returns true if the pattern matches the passed string
        static bool Match(const wchar_t *pattern, const wchar_t * tomatch);

        // Fixes backslashes of the passed path
        static void FixPath(wstring & path);

        // Finds and replaces all occurences of a string in a string (the original string gets updated)
        static void FindAndReplaceAll(string & data, string toSearch, string replaceStr);
 
        // Finds and replaces all occurences of a string in a string (the original string gets updated)
        static void FindAndReplaceAll(wstring & data, wstring toSearch, wstring replaceStr);
        
        // Translates a local file path into a remote file path
        static wstring GetRelativeRemotePath(Folder folder, wstring filePath);

        // Returns the file's path relative to its sync root folder
        static wstring GetRelativePath(Folder folder, wstring filePath);

        // Opens browser for specific actions
        static void OpenBrowser(wstring action, wstring username, wstring relativePath);

        // Returns true if a file exists at the specific path
        static bool FileExists(wstring path);
    };
}
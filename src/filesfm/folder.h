#pragma once
#define UNICODE 1

#include <string>
#include <unordered_set>

using namespace std;

namespace FilesFM {
	class Folder {
		public:

		wstring Username;
		wstring Password;
		wstring Path;
		wstring RemotePath;
		bool CanSync = false;
		bool UseFileLocking = false;
		time_t LastMessagesCheckAt = 0;
		unordered_set<wstring> openFileRelativePaths;
		
		int last_check_tmst = 0; // The last returned last_check_tmst from the API
	};
}

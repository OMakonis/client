#pragma once

#include <string>
#include <vector>
#include <map>

#include "requestparameter.h"


// Standard namespace
using namespace std;

#ifdef FILESFM_REQUEST_IMPL
//#include <curl.h>
#include "C:/CraftRoot/include/curl/curl.h"
#endif

namespace FilesFM {
	class Request
	{	
		private:
			wstring Action;
			wstring Username;
			wstring Password;

#ifdef FILESFM_REQUEST_IMPL
	        // Creates and adds a mime part to the passed mime object
	        static void AddMimePart(curl_mime *mime, string name, string data);

			// Creates and adds a mime part to the passed mime object
	        static void AddMimePart(curl_mime *mime, wstring name, wstring data);

			// Creates and adds a file mime part to the passed mime object
			static void AddFileMimePart(curl_mime *mime, wstring name, wstring path);
#endif

			// Parameters that will be passed along with the request
			vector<RequestParameter> Parameters;

			// Files that will be uploaded with the request
			map<wstring, wstring> Files;

			// Write callback for cURL requests
			static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

			// The base API url
			string BaseUrl = "https://api.files.fm/api/";

			// Dev base API url
			string DevBaseUrl = "https://dev2.failiem.lv/api/";

			// The default script to call
			string DefaultScript = "file/lock.php";

			// Returns a ready to use url for the request
			string GetUrl();

		public:
			// Constructor, requires action, username and password
			Request(wstring action, wstring username, wstring password);

			// Deconstructor
			~Request();

			// The script to call, if left empty DefaultScript will be used instead
			string Script = "";

            // Adds a parameter to this request
			void AddParameter(string name, string data);
			
			// Adds a parameter to this request (wide version)
			void AddParameter(wstring name, wstring data);
			
			// Adds a file to be uploaded
			void AddFile(wstring name, wstring path);

			// Executes the request and writes the returned data to the readBuffer
			bool Execute(string* readBuffer);
	};
}
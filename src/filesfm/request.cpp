#define FILESFM_REQUEST_IMPL
#include "Request.h"
#include "Logger.h"
#include "helpers.h"
#include "debug.h"
#include "logger.h"

#include <sstream>

// Standard namespace
using namespace std;


namespace FilesFM {
    Request::Request(wstring action, wstring username, wstring password) 
    {
        Action = action;
        Username = username;
        Password = password;
    }

    
    Request::~Request()
    {
        // cout << "Calling destructor" << endl;
    }
 
    void Request::AddParameter(wstring name, wstring data)
    {
        auto nameNarrow = Helpers::ToNarrow(name);
        auto dataNarrow = Helpers::ToNarrow(data);

        AddParameter(nameNarrow, dataNarrow);
    }

    void Request::AddParameter(string name, string data)
    {
        RequestParameter parameter;
        parameter.Name = name;
        parameter.Data = data;

        Parameters.push_back(parameter);
    }
    
    void Request::AddFile(wstring name, wstring path)
    {
        Files.insert({name, path});
    }

	bool Request::Execute(string* readBuffer)
    {
        auto url = GetUrl();

        wstringstream ss;
        ss << L"Request::Execute";
        ss << L", url: " << Helpers::ToWide(url);
        ss << L", action: " << Action;
        ss << L", user: " << Username;

        bool success = true;

        CURL *curl = curl_easy_init();

        if ( readBuffer != 0 )
        {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, readBuffer);	
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Avoid invalid certificate warnings
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);


        curl_mime *mime = curl_mime_init(curl);

        AddMimePart(mime, L"action", Action);
        AddMimePart(mime, L"user", Username);
        AddMimePart(mime, L"pass", Password);

        for ( auto p : Parameters )
        {
            AddMimePart(mime, p.Name, p.Data);
            ss << ", " << Helpers::ToWide(p.Name) << ": " << Helpers::ToWide(p.Data);
        }

        // if ( Files.size() > 0 )
        // {
        //     /* tell it to "upload" to the URL */ 
        //     curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        // }

        for ( auto f : Files )
        {
            AddFileMimePart(mime, f.first, f.second);
            ss << ", file:"  << f.first << " (" << f.second << ")";
        }

        // Set MIME OPT
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);


        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 
        // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);

        
        CURLcode res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            success = false;
            goto cleanup;
        }

        ss << ", success: " << (success ? "TRUE" : "FALSE");
        
        if (Action.compare(L"get_unlock_messages") != 0) {
            D(Logger::Out() << ss.str() << endl);
            //D(Logger::Out() << readBuffer->c_str() << endl);
        }

        cout << *readBuffer;

        cleanup:
        curl_easy_cleanup(curl);
        curl_mime_free(mime);

        /*if (Action.compare(L"get_unlock_messages") != 0) {
            D(Logger::Out() << "Request finished" << endl);
        }*/

        return success;
    }

    string Request::GetUrl()
    {
        stringstream ss;
        ss << BaseUrl << (Script.size() > 0 ? Script : DefaultScript);

        return ss.str();
    }

    size_t Request::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    } 

    void Request::AddMimePart(curl_mime *mime, wstring name, wstring data)
    {
        AddMimePart(mime, Helpers::ToNarrow(name), Helpers::ToNarrow(data));
    }

    void Request::AddMimePart(curl_mime *mime, string name, string data)
    {
        auto nameCSTR = name.c_str();
        auto dataCSTR = data.c_str();

        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_name(part, nameCSTR);
        curl_mime_data(part, dataCSTR, CURL_ZERO_TERMINATED);	
    }

    void Request::AddFileMimePart(curl_mime *mime, wstring name, wstring path)
    {
        auto nameNarrow = Helpers::ToNarrow(name);
        auto nameNarrowCSTR = nameNarrow.c_str();
        auto pathNarrow = Helpers::ToNarrow(path);
        auto pathNarrowCSTR = pathNarrow.c_str();

        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_name(part, "uploads[]");
        curl_mime_filedata(part, pathNarrowCSTR);
        curl_mime_filename(part, nameNarrowCSTR);
    
        std::cout << pathNarrowCSTR << endl;
    }
}

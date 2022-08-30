#include "api.h"
#include "filesfm.h"
#include "messaging.h"
#include "debug.h"
#include "logger.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace FilesFM {
    Messaging::Messaging(FilesFM* parent)
    {
        filesFM = parent;
        Start();
    }

    Messaging::~Messaging()
    {
        
    }

    void Messaging::Start()
    {
        _stop = false;

        _thread = new std::thread([this]
		{
			_threadRunning = true;

            D(Logger::Out() << L"Starting messaging thread" << endl);

			while(!_stop)
			{
                CheckMessages();
				std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
			}

            D(Logger::Out() << L"Messaging thread ended" << endl);

			_threadRunning = false;
		});
    }

    void Messaging::Stop()
    {
        _stop = true;
        _thread->join();
    }



    void Messaging::CheckMessages()
    {
        time_t currentTime = time(0);

        list<Folder> folders;

        filesFM->Folders.Mutex.lock();

        for ( auto kvpF : filesFM->Folders.List)
        {
            Folder _f = kvpF.second;
            folders.push_back(_f);
        }
        
        filesFM->Folders.Mutex.unlock();

        for ( auto f : folders )
        {
            if ( difftime(currentTime, f.LastMessagesCheckAt) < 15 )
            {
                continue;
            }

            if ( !f.CanSync || !f.UseFileLocking )
            {
                continue;
            }
            
            auto result = Api::GetUnlockMessages(f);

            filesFM->Folders.List[f.Path].LastMessagesCheckAt = currentTime;

            if ( result.size() > 0 )
            {
                filesFM->Folders.List[f.Path].last_check_tmst = result.begin()->last_check_tmst;
            }

            ProcessMessages(result);
        }
    }

    void Messaging::ProcessMessages(list<UnlockMessage> messages)
    {
        for ( auto m : messages )
        {
            if ( m.Response == true )
            {
                wstring msg = L"The user "+ m.Username +L" has unlocked the file '"+ m.FileName +L"'.\n";
                msg += L"Now you can edit the file without creating conflicting versions.";

				MessageBox(
					NULL,
					msg.c_str(),
					(LPCWSTR)L"File unlocked",
					MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND
				);
            }

            if ( m.Request == true )
            {
                wstring msg = L"The user "+ m.Username +L" is asking you to unlock the file '"+ m.FileName +L"'.\n";
                msg += L"To unlock the file, close it if you currently have opened it or unlock it with the contextmenu Unlock button.";
				
				MessageBox(
					NULL,
					msg.c_str(),
					(LPCWSTR)L"File unlock request",
					MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND
				);
            }
        }
    }
}
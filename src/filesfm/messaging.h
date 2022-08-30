#pragma once

#include "filesfm.fwd.h"
#include <thread>

#include "unlock_message.h"

using namespace std;

namespace FilesFM {
    class Messaging {
        private:
            bool _threadRunning;
            bool _stop;
            thread* _thread;

        public:
            FilesFM* filesFM;

            Messaging(FilesFM* parent);
            ~Messaging();

            // Starts the messaging thread
            void Start();

            // Stops the messaging thread
            void Stop();

            // Checks for new messages
            void CheckMessages();

            // Processes new messages and reacts accordingly
            void ProcessMessages(list<UnlockMessage> messages);
    };
}
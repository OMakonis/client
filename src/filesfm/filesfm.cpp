#include "filesfm.h"

#include <iostream>
#include "api.h"
#include "request.h"
#include "logger.h"
#include "helpers.h"
#include "debug.h"

using namespace std;

namespace FilesFM {
    FilesFM *FilesFM::Instance = 0;

    FilesFM::FilesFM()
    {
        Instance = this;
    }

    FilesFM::~FilesFM()
    {
        D(Logger::Out() << "FilesFM destructor" << endl);

        Instance = 0;

        if ( FileWatcher.Running )
        {
            FileWatcher.Stop();
        }
    }
}
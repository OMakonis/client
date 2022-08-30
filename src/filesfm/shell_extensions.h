#pragma once
#define UNICODE 1

#include "filesfm.fwd.h"

using namespace std;

namespace FilesFM {
class ShellExtensions
{
public:
    ShellExtensions();
    ~ShellExtensions();

    // Reloads shell extensions (currently that means closing all explorer instances and re-launching it afterwards)
    void Reload();
};
}
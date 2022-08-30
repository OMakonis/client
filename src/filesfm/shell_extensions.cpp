#include "shell_extensions.h"
#include <windows.h>
#include <iostream>

using namespace std;

namespace FilesFM {

ShellExtensions::ShellExtensions()
{
}

ShellExtensions::~ShellExtensions()
{
}

void ShellExtensions::Reload()
{
    system("taskkill /f /im explorer.exe");
    system("start explorer.exe");
}
}
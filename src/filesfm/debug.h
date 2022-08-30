#pragma once

#include <iostream>
#include <string>

// https://stackoverflow.com/questions/14251038/debug-macros-in-c

#define DEBUG_BUILD 1

#ifdef DEBUG_BUILD
#define D(x) (x)
#else 
#define D(x) do{}while(0)
#endif
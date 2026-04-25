#include "dxMachine.h"
#include <Windows.h>

extern LARGE_INTEGER ex_freq;
extern LARGE_INTEGER ex_last;
static float delta = 0.0f;

void __InitTimer()
{
    QueryPerformanceFrequency(&ex_freq);
    QueryPerformanceCounter(&ex_last);
}

void __UpdateTimer()
{
    LARGE_INTEGER cur;
    QueryPerformanceCounter(&cur);
    delta = (float)(cur.QuadPart - ex_last.QuadPart) / (float)ex_freq.QuadPart;
    ex_last = cur;
    if (delta > 0.1f) delta = 0.1f;
}

namespace dxMachine
{


float GetDeltaTime()
{
    return delta;
}

}
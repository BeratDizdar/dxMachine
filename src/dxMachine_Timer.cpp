#include "dxMachine.h"
#include <Windows.h>

static LARGE_INTEGER freq;
static LARGE_INTEGER last;
static float delta = 0.0f;

namespace dxMachine
{

void InitTimer()
{
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
}

void UpdateTimer()
{
    LARGE_INTEGER cur;
    QueryPerformanceCounter(&cur);
    delta = (float)(cur.QuadPart - last.QuadPart) / (float)freq.QuadPart;
    last = cur;
    if (delta > 0.1f) delta = 0.1f;
}

float GetDeltaTime()
{
    return delta;
}

}
#include "dxMachine.h"
#include <Windows.h>

static bool curr_keys[256] = { false };
static bool prev_keys[256] = { false };

namespace dxMachine
{

void UpdateInput()
{
    for (int i = 0; i < 256; i++)
    {
        prev_keys[i] = curr_keys[i];
        curr_keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool KeyDown(int k)
{
    return curr_keys[k];
}

bool KeyPressed(int k)
{
    return curr_keys[k] && !prev_keys[k];
}

bool KeyReleased(int k)
{
    return !curr_keys[k] && prev_keys[k];
}

}

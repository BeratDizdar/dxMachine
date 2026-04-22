#include "dxMachine.h"
#include <Windows.h>

namespace dxMachine
{

bool KeyDown(int k)
{
    return (GetAsyncKeyState(k) & 0x8000) != 0;
}

}

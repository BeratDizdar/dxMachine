#include "dxMachine.h"

extern void __InitWindow(const wchar_t* title, int w, int h);
extern void __InitGraphics();
extern void __InitAudio();
extern void __CloseGraphics();
extern void __CloseAudio();
extern void __CloseWindow();

namespace dxMachine
{

    void InitDxMachine(const wchar_t* title, int w, int h)
    {
        // buraya belki hata kontrolü eklerim ama zaten içlerinde var.
        __InitWindow(title, w, h);
        __InitGraphics();
        __InitAudio();
    }

    void CloseDxMachine()
    {
        __CloseAudio();
        __CloseGraphics();
        __CloseWindow();
    }

}

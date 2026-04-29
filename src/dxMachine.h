#pragma once

typedef unsigned int ID;
typedef ID TexID;
typedef ID AudioID;

/* UNUTMADAN
- temel geometri                    (olabilir)
- kamera?                           (şüpheli)
- bit derinliği ekle                (büyük ihtimalle olacak)
- asenkron yükleme fonksiyonları    (büyük ihtimalle olacak)
- screenshot?                       (olabilir)
- memory management?                (sanırım gerekecek)
*/

/* TODO
    // WaitKey();
    //void DrawLine(int x1, int y1, int x2, int y2, unsigned long color);
    //void DrawRect(int x, int y, int w, int h, unsigned long color, bool bFill);
    //void SetAudioVolume(AudioID audio, float volume);
    //void SetAudioPitch(AudioID audio, float pitch);
*/

namespace dxMachine
{

    /* API */

    void InitDxMachine(const wchar_t* title, int w, int h);
    void CloseDxMachine();

    /* Platform */

    void ToggleFullscreen();
    bool ProcessMessage();
    float GetDeltaTime();
    bool KeyDown(int k);
    bool KeyPressed(int k);
    bool KeyReleased(int k);
    void MousePos(int* x, int* y);
    bool LeftMouseButtonState();
    bool RightMouseButtonState();

    /* Renderer */

    void ScreenEmulation(int virtual_w, int virtual_h);
    void ScreenClear(unsigned long color);
    void ScreenFlip();
    TexID TextureFromFile(const char* path);
    TexID TextureFromTexture(TexID root_id, int x, int y, int w, int h);
    void DrawSprite(TexID texture, int x, int y);
    void DrawSpriteEx(TexID texture, int x, int y, int w, int h);

    /* Audio */

    AudioID AudioFromOGG(const char* path);
    void PlayAudio(AudioID audio);
    void StopAudio(AudioID audio);

    /* Compute */

    bool CheckAABB(int ax, int ay, int aw, int wh, int bx, int by, int bw, int bh);

}
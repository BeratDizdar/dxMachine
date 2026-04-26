#pragma once

typedef unsigned int ID;
typedef ID TexID;
typedef ID AudioID;

/* UNUTMADAN
- text ve font işine bir hal çare   (şüpheli)
- temel geometri                    (olabilir)
- kamera?                           (şüpheli)
- bit derinliği ekle                (büyük ihtimalle olacak)
- Hata mesajları programı bitirmeli (kesin olacak)
- asenkron yükleme fonksiyonları    (büyük ihtimalle olacak)
- screenshot?                       (olabilir)
- memory management?                (sanırım gerekecek)
*/

namespace dxMachine
{

/* Window */

void InitWindow(const wchar_t* title, int w, int h, int max_texture);
void CloseWindow();
bool ProcessMessage();

/* Timer */

float GetDeltaTime();

/* Renderer */

void ScreenEmulation(int virtual_w, int virtual_h);
void ScreenClear(unsigned long color);
void ScreenFlip();
TexID TextureFromFile(const char* path);
TexID TextureFromTexture(TexID root_id, int x, int y, int w, int h);
void DrawSprite(TexID texture, int x, int y);
void DrawSpriteEx(TexID texture, int x, int y, int w, int h);

/* Input */

bool KeyDown(int k);
bool KeyPressed(int k);
bool KeyReleased(int k);
// WaitKey();
// MousePos(int* x, int* y);
// MouseInput();

/* Audio */

AudioID AudioFromOGG(const char* path);
void PlayAudio(AudioID audio);
void StopAudio(AudioID audio);

/* Compute */

bool CheckAABB(int ax, int ay, int aw, int wh, int bx, int by, int bw, int bh);

}
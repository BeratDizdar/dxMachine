#pragma once

typedef unsigned int ID;
typedef ID TexID;
typedef ID AudioID;

/* UNUTMA
- ASENKRON YÜKLEME FONKSİYONLARI EKLE
- THREADPOOL işi ile uğraştın götünde patladı fikir
*/

namespace dxMachine
{

/* Window */

void InitWindow(const wchar_t* title, int w, int h, int max_texture);
void CloseWindow();
bool WindowActive();
void ShowMessage(const wchar_t* title, const wchar_t* text);
void ProcessMessage();

/* Timer */

float GetDeltaTime();

/* Renderer */

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

/* Audio */

AudioID AudioFromOGG(const char* path);
void PlayAudio(AudioID audio);
void StopAudio(AudioID audio);

}
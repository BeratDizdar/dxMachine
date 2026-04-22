#pragma once

typedef unsigned int ID;
typedef ID TexID;

namespace dxMachine
{

/* WINDOW */

void InitWindow(const wchar_t* title, int w, int h);
void CloseWindow();
bool WindowActive();

void* GetWindow();
void ShowMessage(const wchar_t* title, const wchar_t* text);

void ProcessMessage();

/* TIMER */

void InitTimer();
void UpdateTimer();
float GetDeltaTime();

/* RENDERER */

void InitGraphics(void* window, int max_texture);
void CloseGraphics();

void ScreenClear(unsigned long color);
void ScreenFlip();

void DrawSprite(TexID texture, int x, int y);
void DrawSpriteEx(TexID texture, int x, int y, int w, int h);

TexID LoadTextureFromFile(const char* path);
TexID CreateTextureFromTexture(TexID root_id, int x, int y, int w, int h);

/* INPUT */

bool KeyDown(int k);

/* AUDIO */



}
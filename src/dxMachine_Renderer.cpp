#include "dxMachine.h"
#include "depen/stb_image.h"
#include <Windows.h>
#include <d3d9.h>

extern HWND ex_hwnd;
static IDirect3D9* root;
static IDirect3DDevice9* dev;
static IDirect3DSurface9* surface;
static IDirect3DSurface9* backbuffer;

struct Vertex2D {
    float x, y, z, rhw;
    float u, v;
};

struct Texture {
    IDirect3DTexture9* ptr;
    int w, h; // size
    float u0, v0, u1, v1;
};

// SANIRIM UZUN İSİMLER KOYMA İŞİNİ ÇÖZMEM LAZIM

static Texture* tex;
static int cur_tex_id = 0;
static int _max_texture = 0;
static int current_batch_texture_id = 0;
static Vertex2D vertex_batch[4000]{};
static int current_batch = 0;

static void __Flush() {
    dev->SetTexture(0, tex[current_batch_texture_id].ptr);
    dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
    dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, current_batch - 2, vertex_batch, sizeof(Vertex2D));
    current_batch = 0;
}

void __InitGraphics(int max_texture)
{
    root = ::Direct3DCreate9(D3D_SDK_VERSION);
    if (root == nullptr) ::MessageBoxW(NULL, L"Direct3D9 is null", L"HATA", MB_ICONERROR | MB_OK);
    
    D3DCAPS9 deviceCaps{};
    root->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps);
    int vertexProcessing{};

    if ((deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
        vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    D3DPRESENT_PARAMETERS param{};
    param.Windowed = TRUE;
    param.SwapEffect = D3DSWAPEFFECT_DISCARD;
    param.BackBufferFormat = D3DFMT_X8R8G8B8;
    param.BackBufferCount = 1;
    param.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    root->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, ex_hwnd,
        vertexProcessing | D3DCREATE_PUREDEVICE, &param, &dev);
    if (dev == nullptr) ::MessageBoxW(NULL, L"Direct3DDevice9 is null", L"HATA", MB_ICONERROR | MB_OK);

    RECT rect{};
    if (::GetClientRect(ex_hwnd, &rect)) {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        dev->CreateRenderTarget(width, height, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &surface, NULL);
    }

    dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

    _max_texture = max_texture;
    tex = (Texture*)calloc(_max_texture, sizeof(Texture));
}

void __CloseGraphics()
{
    if (surface) surface->Release();
    if (backbuffer) backbuffer->Release();
    for (int i = 0; i < _max_texture; i++) {
        if (tex[i].ptr) tex[i].ptr->Release();
    }
    if (dev) dev->Release();
    if (root) root->Release();
}

namespace dxMachine
{

void ScreenClear(unsigned long color)
{
    dev->Clear(0, 0, D3DCLEAR_TARGET, color, 1.0f, 0);
    dev->BeginScene();
}

void ScreenFlip()
{
    __Flush();
    dev->EndScene();
    dev->Present(0, 0, 0, 0);
}

void DrawSprite(TexID texture, int x, int y)
{
    if (texture != current_batch_texture_id || current_batch > 3990) {
        __Flush();
        current_batch_texture_id = texture;
    }

    float u0 = tex[texture].u0;
    float v0 = tex[texture].v0;
    float u1 = tex[texture].u1;
    float v1 = tex[texture].v1;
    float w = tex[texture].w;
    float h = tex[texture].h;

    if (current_batch > 0) 
    {
        vertex_batch[current_batch++] = vertex_batch[current_batch - 1];
        vertex_batch[current_batch++] = Vertex2D { (float)x, (float)y, 0.0f, 1.0f, u0, v0 };
    }

    vertex_batch[current_batch++] = Vertex2D {(float)x, (float)y, 0.0f, 1.0f, u0, v0};
    vertex_batch[current_batch++] = Vertex2D {(float)x+w, (float)y, 0.0f, 1.0f, u1, v0};
    vertex_batch[current_batch++] = Vertex2D {(float)x, (float)y+h, 0.0f, 1.0f, u0, v1};
    vertex_batch[current_batch++] = Vertex2D {(float)x+w, (float)y+h, 0.0f, 1.0f, u1, v1};
}

void DrawSpriteEx(TexID texture, int x, int y, int w, int h)
{
    if (texture != current_batch_texture_id || current_batch > 3990) {
        __Flush();
        current_batch_texture_id = texture;
    }
    float u0 = tex[texture].u0;
    float v0 = tex[texture].v0;
    float u1 = tex[texture].u1;
    float v1 = tex[texture].v1;

    if (current_batch > 0) 
    {
        vertex_batch[current_batch++] = vertex_batch[current_batch - 1];
        vertex_batch[current_batch++] = Vertex2D { (float)x, (float)y, 0.0f, 1.0f, u0, v0 };
    }

    vertex_batch[current_batch++] = Vertex2D {(float)x,          (float)y,          0.0f, 1.0f,u0,v0};
    vertex_batch[current_batch++] = Vertex2D {(float)x+(float)w, (float)y,          0.0f, 1.0f,u1,v0};
    vertex_batch[current_batch++] = Vertex2D {(float)x,          (float)y+(float)h, 0.0f, 1.0f,u0,v1};
    vertex_batch[current_batch++] = Vertex2D {(float)x+(float)w, (float)y+(float)h, 0.0f, 1.0f,u1,v1};
}

TexID TextureFromFile(const char* path)
{
    if (cur_tex_id >= _max_texture) {
        ::MessageBoxW(NULL, L"Texture Overflow", L"HATA", MB_ICONERROR | MB_OK);
        return 0;
    }

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

    if (!data) {
        ::MessageBoxW(NULL, L"Dosya Bulunamadı", L"HATA", MB_ICONERROR | MB_OK);
        return 0;
    }

    if (FAILED(dev->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex[cur_tex_id].ptr, NULL))) {
        stbi_image_free(data);
        ::MessageBoxW(NULL, L"Texture Yaratılamadı", L"HATA", MB_ICONERROR | MB_OK);
        return 0;
    }

    D3DLOCKED_RECT rect;
    tex[cur_tex_id].ptr->LockRect(0, &rect, NULL, 0);

    unsigned char* dest = (unsigned char*)rect.pBits;
    for (int i = 0; i < width * height * 4; i += 4) {
        dest[i + 0] = data[i + 2];
        dest[i + 1] = data[i + 1];
        dest[i + 2] = data[i + 0];
        dest[i + 3] = data[i + 3];
    }

    tex[cur_tex_id].ptr->UnlockRect(0);
    stbi_image_free(data);

    tex[cur_tex_id].w = width;
    tex[cur_tex_id].h = height;
    tex[cur_tex_id].u0 = 0.0f;
    tex[cur_tex_id].v0 = 0.0f;
    tex[cur_tex_id].u1 = 1.0f;
    tex[cur_tex_id].v1 = 1.0f;

    return cur_tex_id++;
}

TexID TextureFromTexture(TexID root_id, int x, int y, int w, int h)
{
    if (cur_tex_id >= _max_texture) {
        ::MessageBoxW(NULL, L"Texture Overflow", L"HATA", MB_ICONERROR | MB_OK);
        return 0;
    }
    
    tex[cur_tex_id].ptr = tex[root_id].ptr;
    tex[cur_tex_id].ptr->AddRef();

    tex[cur_tex_id].w = w;
    tex[cur_tex_id].h = h;

    float total_w = (float)tex[root_id].w;
    float total_h = (float)tex[root_id].h;

    tex[cur_tex_id].u0 = x / total_w;
    tex[cur_tex_id].v0 = y / total_h;
    tex[cur_tex_id].u1 = (x + w) / total_w;
    tex[cur_tex_id].v1 = (y + h) / total_h;

    return cur_tex_id++;
}

}
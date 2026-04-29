#include "dxMachine.h"
#include "depen/stb_image.h"
#include <Windows.h>
#include <d3d9.h>

#define DXM_ERROR(msg) ::MessageBoxW(NULL, L ## msg, L"HATA", MB_ICONERROR | MB_OK);
#define DXM_CHECK(body, msg) do{ if(body != D3D_OK) { DXM_ERROR(msg); ::exit(-1);} }while(0)

extern HWND wnd_handler;
extern int wnd_w;
extern int wnd_h;

static IDirect3D9* root;
static IDirect3DDevice9* dev;
static IDirect3DSurface9* backbuffer;
static IDirect3DTexture9* emulation_texture;
static IDirect3DSurface9* emulation_surface;
static int v_w, v_h;

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
static int texture_cap = 0;
static int current_batch_texture_id = 0;
static Vertex2D vertex_batch[4000]{};
static int current_batch = 0;

static struct gfx {

    void flush() {
        dev->SetTexture(0, tex[current_batch_texture_id].ptr);
        dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
        dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, current_batch - 2, vertex_batch, sizeof(Vertex2D));
        current_batch = 0;
    }

    void init_d3d9() {
        root = ::Direct3DCreate9(D3D_SDK_VERSION);
        if (root == nullptr)
        {
            DXM_ERROR("Direct3D9 arayüzü yaratılamadı");
            exit(-1);
        }
        
        D3DCAPS9 deviceCaps{};
        root->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps);

        int vertexProcessing
            = ((deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
            ? D3DCREATE_HARDWARE_VERTEXPROCESSING
            : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

        D3DPRESENT_PARAMETERS param{};
        param.Windowed = TRUE;
        param.SwapEffect = D3DSWAPEFFECT_DISCARD;
        param.BackBufferFormat = D3DFMT_X8R8G8B8;
        param.BackBufferCount = 1;
        param.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        DXM_CHECK(root->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd_handler,
            vertexProcessing | D3DCREATE_PUREDEVICE, &param, &dev)
            ,"Direct3DDevice9 arayüzü yaratılamadı");

        dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
    }

    void set_state() {
        dev->SetRenderState(D3DRS_LIGHTING, FALSE);
        dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

        dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    }

    void free_d3d9() {
        if (emulation_surface) emulation_surface->Release();
        if (emulation_texture) emulation_texture->Release();
        if (backbuffer) backbuffer->Release();
        for (int i = 0; i < texture_cap; i++) {
            if (tex[i].ptr) tex[i].ptr->Release();
        }
        if (dev) dev->Release();
        if (root) root->Release();
    }

} gfx;

void __InitGraphics()
{
    gfx.init_d3d9();
    gfx.set_state();

    RECT rect{};
    if (::GetClientRect(wnd_handler, &rect)) {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        dxMachine::ScreenEmulation(width, height);
    }

    tex = (Texture*)calloc(8, sizeof(Texture));
}

void __CloseGraphics()
{
    gfx.free_d3d9();
}

namespace dxMachine
{

    void ScreenEmulation(int virtual_w, int virtual_h)
    {
        v_w = virtual_w;
        v_h = virtual_h;

        if (emulation_surface) emulation_surface->Release();
        if (emulation_texture) emulation_texture->Release();

        dev->CreateTexture(
            virtual_w, virtual_h, 
            1, 
            D3DUSAGE_RENDERTARGET, 
            D3DFMT_X8R8G8B8, 
            D3DPOOL_DEFAULT, 
            &emulation_texture,
            NULL
        );

        emulation_texture->GetSurfaceLevel(0, &emulation_surface);
        dev->SetRenderTarget(0, emulation_surface);
    }

    void ScreenClear(unsigned long color)
    {
        dev->Clear(0, 0, D3DCLEAR_TARGET, color, 1.0f, 0);
        dev->BeginScene();
    }

    void ScreenFlip()
    {
        gfx.flush();
        dev->EndScene();

        dev->SetRenderTarget(0, backbuffer);
        dev->BeginScene();
        dev->SetTexture(0, emulation_texture);

        float fx = -0.5f;
        float fy = -0.5f;
        float fw = (float)wnd_w;
        float fh = (float)wnd_h;

        Vertex2D full_screen[4];
        full_screen[0] = { fx,      fy,      0.0f, 1.0f, 0.0f, 0.0f };
        full_screen[1] = { fx + fw, fy,      0.0f, 1.0f, 1.0f, 0.0f };
        full_screen[2] = { fx,      fy + fh, 0.0f, 1.0f, 0.0f, 1.0f };
        full_screen[3] = { fx + fw, fy + fh, 0.0f, 1.0f, 1.0f, 1.0f };

        dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
        dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, full_screen, sizeof(Vertex2D));
        dev->EndScene();
        dev->Present(0, 0, 0, 0);

        dev->SetTexture(0, NULL);
        dev->SetRenderTarget(0, emulation_surface);
    }

    void DrawSprite(TexID texture, int x, int y)
    {
        if (texture != current_batch_texture_id || current_batch > 3990) {
            gfx.flush();
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
            gfx.flush();
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

    TexID TextureFromFile(const char *path)
    {
        if (cur_tex_id >= texture_cap) {
            texture_cap += 8;
            tex = (Texture*)realloc(tex, sizeof(Texture) * texture_cap);
            if (tex == nullptr)
            {
                DXM_ERROR("Texture için ekstra yer ayrılamadı!");
                ::exit(-1);
            }
            memset(&tex[texture_cap - 8], 0, 8 * sizeof(Texture));
        }

        int width, height, channels;
        unsigned char* data = ::stbi_load(path, &width, &height, &channels, 4);

        if (!data) {
            DXM_ERROR("Dosya Bulunamadı!");
            ::exit(-1);
        }

        if (FAILED(dev->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex[cur_tex_id].ptr, NULL))) {
            ::stbi_image_free(data);
            DXM_ERROR("Texture Yaratılamadı!");
            ::exit(-1);
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
        ::stbi_image_free(data);

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
        if (cur_tex_id >= texture_cap) {
            texture_cap += 8;
            tex = (Texture*)realloc(tex, sizeof(Texture) * texture_cap);
            if (tex == nullptr)
            {
                DXM_ERROR("Texture için ekstra yer ayrılamadı!");
                ::exit(-1);
            }
            memset(&tex[texture_cap - 8], 0, 8 * sizeof(Texture));
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
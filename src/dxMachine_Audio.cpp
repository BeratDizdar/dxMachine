/* REF
- https://learn.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-apis-portal
*/

#include "dxMachine.h"
#include <xaudio2.h>
#include "depen/stb_vorbis.c"

static IXAudio2* root = nullptr;
static IXAudio2MasteringVoice* mv = nullptr;

struct AudioNode {
    IXAudio2SourceVoice* voice;
    XAUDIO2_BUFFER buffer;
    short* decoded_data;
};

static AudioNode g_Audio[256];
static int cur_audio_id = 0;

void __InitAudio()
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    XAudio2Create(&root, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (root == nullptr) {
        ::MessageBoxW(NULL, L"XAudio2 yaratılamadı!", L"HATA", MB_ICONERROR | MB_OK);
        return;
    }

    root->CreateMasteringVoice(&mv);
    if (mv == nullptr) {
        ::MessageBoxW(NULL, L"Hoparlör çıkışı bulunamadı!", L"HATA", MB_ICONERROR | MB_OK);
        return;
    }
}

void __CloseAudio()
{
    for (int i = 0; i < cur_audio_id; i++) {
        if (g_Audio[i].voice) g_Audio[i].voice->DestroyVoice();
        if (g_Audio[i].decoded_data) free(g_Audio[i].decoded_data);
    }

    if (mv) mv->DestroyVoice();
    if (root) root->Release();
    CoUninitialize();
}

namespace dxMachine
{

AudioID AudioFromOGG(const char* path)
{
    if (cur_audio_id >= 256) return -1;

    int channels, sampleRate;
    short* decoded;

    int num_samples = stb_vorbis_decode_filename(path, &channels, &sampleRate, &decoded);
    if (num_samples < 0) {
        ::MessageBoxW(NULL, L".ogg dosyası okunamadı!", L"HATA", MB_ICONERROR | MB_OK);
        return -1;
    }

    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = channels;
    wfx.nSamplesPerSec = sampleRate;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (channels * 16) / 8;
    wfx.nAvgBytesPerSec = sampleRate * wfx.nBlockAlign;

    IXAudio2SourceVoice* voice = nullptr;
    root->CreateSourceVoice(&voice, &wfx);

    XAUDIO2_BUFFER buffer = {0};
    buffer.AudioBytes = num_samples * channels * sizeof(short);
    buffer.pAudioData = (BYTE*)decoded;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = 0;

    g_Audio[cur_audio_id].voice = voice;
    g_Audio[cur_audio_id].buffer = buffer;
    g_Audio[cur_audio_id].decoded_data = decoded;

    return cur_audio_id++;
}

void PlayAudio(AudioID audio)
{
    if (audio < 0 || audio >= cur_audio_id) return;

    g_Audio[audio].voice->Stop(0);
    g_Audio[audio].voice->FlushSourceBuffers();
    g_Audio[audio].voice->SubmitSourceBuffer(&g_Audio[audio].buffer);
    g_Audio[audio].voice->Start(0);
}

void StopAudio(AudioID audio)
{
    if (audio < 0 || audio >= cur_audio_id) return;

    g_Audio[audio].voice->Stop(0);
}

}
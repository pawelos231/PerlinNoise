#include <iostream>
#include <string>
#include <algorithm>
#include "olcConsoleGameEngine.h"

class PerlinNoiseProject : public olcConsoleGameEngine {
public:
    PerlinNoiseProject() {
        m_sAppName = L"Perlin Noise";
    }
private:

    //2D noise

    float* fNoiseSeed2D = nullptr;
    float* fPerlinNoise2D = nullptr;
    int nOutputHeight = 256;
    int nOutputWidth = 256;

    //1D noise
    float *fNoiseSeed1D = nullptr;
    float *fPerlinNoise1D = nullptr;
    int nOutputSize = 256;
    
    int nOctaveCount = 1;
    float fScalingBias = 2.0f;
    int nMode = 1;

    virtual bool OnUserCreate() {

        nOutputWidth = ScreenWidth();
        nOutputHeight = ScreenHeight();

        fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
        fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];
        for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


        nOutputSize = ScreenWidth();
        fNoiseSeed1D = new float[nOutputSize];
        fPerlinNoise1D = new float[nOutputSize];
        for (int i = 0; i < nOutputSize; i++) {
            fNoiseSeed1D[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }
        return true;
    };
    virtual bool OnUserUpdate(float ElapsedTime) {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

        if (m_keys[VK_SPACE].bReleased) {
            nOctaveCount++;
        }
        if (nOctaveCount == 9) {
            nOctaveCount = 1;
        };
        if (m_keys[L'1'].bReleased) {
            nMode = 1;
        };

        if (m_keys[L'2'].bReleased) {
            nMode = 2;
        };

        if (m_keys[L'Q'].bReleased) {
            fScalingBias += 0.2f;
        }

        if (m_keys[L'A'].bReleased) {
            fScalingBias -= 0.2f;
        }

        if (fScalingBias < 0.2f) {
            fScalingBias = 0.2f;
        }

        if (m_keys[L'Z'].bReleased) {
            nOctaveCount++;
        }
        if (nMode == 1) {

            if (m_keys[L'Z'].bReleased) // Noise Between 0 and +1
                for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

            if (m_keys[L'X'].bReleased) // Noise Between -1 and +1
                for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;

            GeneratePerlinNoise1D(nOutputSize, fNoiseSeed1D, nOctaveCount, fScalingBias, fPerlinNoise1D);
            for (int x = 0; x < nOutputSize; x++) {
                int y = -(fPerlinNoise1D[x] * static_cast<float>(ScreenHeight() / 2.0f)) + static_cast<float>(ScreenHeight() / 2.0f);
                if (y < ScreenHeight() / 2)
                    for (int f = y; f < ScreenHeight() / 2; f++)
                        Draw(x, f, PIXEL_SOLID, FG_GREEN);
                else
                    for (int f = ScreenHeight() / 2; f <= y; f++)
                        Draw(x, f, PIXEL_SOLID, FG_RED);
            }
        };

        if (nMode == 2) {
            if (m_keys[L'Z'].bReleased) // Noise Between 0 and +1
                for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

            GeneratePerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

            for (int x = 0; x < nOutputWidth; x++)
            {
                for (int y = 0; y < nOutputHeight; y++)
                {
                    short bg_col, fg_col;
                    wchar_t sym;
                    int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 12.0f);
                    switch (pixel_bw)
                    {
                    case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

                    case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
                    case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
                    case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
                    case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

                    case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
                    case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
                    case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
                    case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

                    case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
                    case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
                    case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
                    case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
                    }

                    Draw(x, y, sym, fg_col | bg_col);
                }
            }

        }

        return true;
    }

    void GeneratePerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias,  float* fOutput) {
        for (int i = 0; i < nCount; i++) {
            float fNoise = 0.0f;
            float fScale = 1.0f;
            float fScaleAcc = 0.0f;

            for (int j = 0; j < nOctaves; j++) {
                int nPitch = nCount >> j;
                int nSample1 = (i / nPitch) * nPitch;
                int nSample2 = (nSample1 + nPitch) % nCount;

                float fBlend = static_cast<float>(i - nSample1) / static_cast<float>(nPitch);
                float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];

                fNoise += fSample * fScale;
                fScaleAcc += fScale;
                fScale = fScale / fBias;

            }
            fOutput[i] = fNoise / fScaleAcc;
        }
    }
    void GeneratePerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput)
    {
        // Used 1D Perlin Noise
        for (int x = 0; x < nWidth; x++) {
            for (int y = 0; y < nHeight; y++)
            {
                float fNoise = 0.0f;
                float fScaleAcc = 0.0f;
                float fScale = 1.0f;

                for (int o = 0; o < nOctaves; o++)
                {
                    int nPitch = nWidth >> o;
                    int nSampleX1 = (x / nPitch) * nPitch;
                    int nSampleY1 = (y / nPitch) * nPitch;

                    int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
                    int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

                    float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
                    float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

                    float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
                    float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

                    fScaleAcc += fScale;
                    fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
                    fScale = fScale / fBias;
                }

                // Scale to seed range
                fOutput[y * nWidth + x] = fNoise / fScaleAcc;
            }
        }

    };

};




int main()
{
    PerlinNoiseProject Perlin;
    Perlin.ConstructConsole(256, 256, 3, 3);
    Perlin.Start();
    return 0;
}



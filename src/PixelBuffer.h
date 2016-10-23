#ifndef PIXELBUFFER_H
#define PIXELBUFFER_H

#include <string>
#include <vector>

typedef struct Pixel{
    //all values 0-255
    int r;
    int g;
    int b;
    int a;
    Pixel()
        : r(0), g(0), b(0), a(1) {};
    Pixel(int rr, int gg, int bb, int aa)
        : r(rr), g(gg), b(bb), a(aa) {}
} Pixel;

class PixelBuffer
{
public:
    PixelBuffer();
    PixelBuffer(int w, int h);
    ~PixelBuffer() = default;
    void LoadFromImage(std::string path);
    std::vector<Pixel> GetPixelData();
    int GetWidth(){ return m_width; }
    int GetHeight(){ return m_height; }
    void InitPixelData();
    void ClearPixelBuffer();
    std::string GetAsciiRepresentation();
private:
    std::vector<Pixel> m_pixelVector;
    int m_width = 0;
    int m_height = 0;
    std::string characterIntensityScale;

    std::pair<Pixel, float> GetPartitionData(int x, int y);
};

#endif
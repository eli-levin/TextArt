#include "PixelBuffer.h"
#include <stdio.h>
#include <string.h>
#include <png.h>
#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>

// This string represents greyscale intesities from black -> white using ascii characters.
// Source: Paul Bourke (1997) http://paulbourke.net/dataformats/asciiart/
#define CHARACTER_INTENSITY_SCALE "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "

#define PARTITION_SIZE 8

PixelBuffer::PixelBuffer()
{}

PixelBuffer::PixelBuffer(int width, int height)
    : m_width(width),
    m_height(height),
    characterIntensityScale(CHARACTER_INTENSITY_SCALE)
{}

void PixelBuffer::LoadFromImage(std::string path)
{
    png_image image;
    memset(&image, 0, (sizeof image));
    image.version = PNG_IMAGE_VERSION;
    
    if (png_image_begin_read_from_file(&image, path.c_str()))
    {
        
        m_width = image.width;
        m_height = image.height;
        m_pixelVector.clear();
        
        png_bytep buffer;
        image.format = PNG_FORMAT_RGBA;
        buffer = new png_byte[PNG_IMAGE_SIZE(image)];
        
        if (buffer && png_image_finish_read(&image, NULL, buffer, 0, NULL))
        {
            
            for (unsigned int y = 0; y < image.height; y++)
            {
                for (unsigned int x = 0; x < image.width; x++)
                {
                    int r, g, b, a = 0;
                    r = (int)buffer[(y*image.width*4)+(x*4)];
                    g = (int)buffer[(y*image.width*4)+(x*4)+1];
                    b = (int)buffer[(y*image.width*4)+(x*4)+2];
                    a = (int)buffer[(y*image.width*4)+(x*4)+3];
                    m_pixelVector.push_back(Pixel(r,g,b,a));
                }
            }
            
        }
        
        delete[] buffer;
    }
}

std::vector<Pixel> PixelBuffer::GetPixelData()
{
    return m_pixelVector;
}

void PixelBuffer::InitPixelData()
{
    int pixelCount = m_width * m_height;
    for (int i = 0; i < pixelCount; i++)
    {
        m_pixelVector.push_back(Pixel(0,0,0,0));
    }
}

void PixelBuffer::ClearPixelBuffer()
{
    m_pixelVector.clear();
}

std::pair<Pixel, float> PixelBuffer::GetPartitionData(int x, int y)
{
    Pixel colAvg;
    float avg = 0;
    
    int start = (PARTITION_SIZE * m_width * y) + (PARTITION_SIZE*x);
    for (int i = 0; i < PARTITION_SIZE; i++)
    {
        for (int j = 0; j < PARTITION_SIZE; j++)
        {
            int rel = m_width*i + j;
            Pixel col = m_pixelVector.at(start + rel);

            //color
            colAvg.r += col.r;
            colAvg.g += col.g;
            colAvg.b += col.b;

            //intensity
            float intensity = (float)col.r + (float)col.g + (float)col.b;
            avg += (intensity/(3.0f*255.0f));
        }
    }

    // average out color of partition
    colAvg.r = colAvg.r/(PARTITION_SIZE * PARTITION_SIZE);
    colAvg.g = colAvg.g/(PARTITION_SIZE * PARTITION_SIZE);
    colAvg.b = colAvg.b/(PARTITION_SIZE * PARTITION_SIZE);

    // average out intensity
    avg = avg/(PARTITION_SIZE * PARTITION_SIZE);

    return std::make_pair(colAvg, avg);
}

std::string PixelBuffer::GetAsciiRepresentation()
{
    //
    // Go through each set of 8x8 pixels and append a char to the output string
    // Go through pixels by partitioning the buffer into 8x8 pixel chunks (size of ascii character)
    // Trim image by only looking at stuff in chunk bounds
    //
    const int cols = m_width/PARTITION_SIZE;
    const int rows = m_height/PARTITION_SIZE;
    
    char outstring[cols*rows + m_height]; //plus room for newlines
    std::ostringstream obuf;
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            characterIntensityScale = std::string(CHARACTER_INTENSITY_SCALE);
            
            std::pair<Pixel, float> partitionInfo = GetPartitionData(j, i);
            Pixel pixCol = partitionInfo.first;
            float intensity = partitionInfo.second;

            intensity = (1.0f - intensity);
            int charIndex = (intensity * (characterIntensityScale.size()-1));
            outstring[i*cols + j] = characterIntensityScale.at(charIndex);

            // Get 8-bit color for this character.
            int rv = (int) std::round(((float)pixCol.r/255.)*7.);
            int gv = (int) std::round(((float)pixCol.g/255.)*7.);
            int bv = (int) std::round(((float)pixCol.b/255.)*3.);

            int colour = 0;
            colour += rv << 5;
            colour += gv << 2;
            colour += bv;

            obuf << "\033[38;5;" << colour << "m" << characterIntensityScale.at(charIndex) << "\033[0m";
        }
        outstring[i*cols + cols-1] = '\n';

        //db
        obuf << std::endl;
    }

    //db
    std::cout << "++++++" << std::endl << obuf.str() << std::endl << "++++++" << std::endl;

    return outstring;
}
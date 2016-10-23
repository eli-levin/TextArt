#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "PixelBuffer.h"

// This string represents greyscale intesities from black -> white using ascii characters.
// Source: Paul Bourke (1997) http://paulbourke.net/dataformats/asciiart/
#define CHARACTER_INTENSITY_SCALE "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "
#define LINE_SEPARATOR "********************************************************************************"

int main(int argc, char const *argv[])
{
    // asciify.exe <absolute_path_of_image> TODO: <density_of_conversion 25|50|100 : int>
    if (argc != 2)
    {
        printf("Error: provide exactly one argument (absolute path of image)\n");
        return 1;
    }
    // TODO: parse and check path
    std::string absFilePath(argv[1]);

    PixelBuffer imageData;
    imageData.LoadFromImage(absFilePath);
    std::string asciiRepresentation = imageData.GetAsciiRepresentation(); //TODO: add chunk size
    
    std::cout << "---Image in ASCII text---" << std::endl << LINE_SEPARATOR << std::endl;
    std::cout << asciiRepresentation << LINE_SEPARATOR << std::endl;

    return 0;
};
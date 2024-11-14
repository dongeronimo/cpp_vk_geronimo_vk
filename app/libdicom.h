#pragma once
#include <cassert>
#include <stdio.h>
#include <stdint.h>


namespace volumerendering {
    struct LoadDicomImageResult {
        uint32_t numberOfFiles;
        float spacingX;
        float spacingY;
        float thickness;
        uint32_t rows;
        uint32_t cols;
        short* data;
    };
    

    LoadDicomImageResult LoadDicomImage(const char* dir);
    //extern 

    void InitializeFunctions();
}
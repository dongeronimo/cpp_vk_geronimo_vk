#pragma once
#include <Windows.h>
#include <cassert>
#include <stdio.h>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN

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
    typedef LoadDicomImageResult(*load_dicom_image_t)(const char* dir);

    load_dicom_image_t LoadDicomImage;

    template <typename t>
    t LoadFN(HMODULE _module, const char* name) {
        t ptr = (t)GetProcAddress(_module, name);
        assert(ptr != nullptr);
        return ptr;
    }
    void InitializeFunctions() {
        HMODULE m = LoadLibrary("tomographer.dll");//it assumes that the dll is at the same folder that the exe is
        assert(m != 0);
        volumerendering::LoadDicomImage = LoadFN<volumerendering::load_dicom_image_t>(m, "LoadDicomImage");
    }
}
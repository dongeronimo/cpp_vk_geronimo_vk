#include "libdicom.h"
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

template <typename t>
t LoadFN(HMODULE _module, const char* name) {
    t ptr = (t)GetProcAddress(_module, name);
    assert(ptr != nullptr);
    return ptr;
}

namespace volumerendering 
{
    typedef LoadDicomImageResult(*load_dicom_image_t)(const char* dir);
    load_dicom_image_t _loadDicomImage;
    LoadDicomImageResult volumerendering::LoadDicomImage(const char* dir)
    {
        return _loadDicomImage(dir);
    }
}
void volumerendering::InitializeFunctions()
{
    HMODULE m = LoadLibrary("tomographer.dll");//it assumes that the dll is at the same folder that the exe is
    assert(m != 0);
    volumerendering::_loadDicomImage = LoadFN<volumerendering::load_dicom_image_t>(m, "LoadDicomImage");
}

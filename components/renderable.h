#pragma once
#include "transform.h"
#include "mesh.h"
namespace components
{
    class Renderable : public Transform {
    public:
        Renderable(const std::string& n, Mesh& mesh);
        const Mesh& mMesh;
    };

}
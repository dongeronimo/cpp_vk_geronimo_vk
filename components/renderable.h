#pragma once
#include "transform.h"
#include "mesh.h"
#include "solid_phong_pipeline.h"
namespace components
{
    class Renderable : public Transform, public ModelMatrixUniform {
    public:
        Renderable(const std::string& n, Mesh& mesh);
        const Mesh& mMesh;
    };

}
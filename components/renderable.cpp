#include "renderable.h"
namespace components {
    Renderable::Renderable(const std::string& n, Mesh& mesh)
        :Transform(n), mMesh(mesh)
    {
    }
}
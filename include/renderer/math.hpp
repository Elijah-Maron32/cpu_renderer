#include <renderer/typeDefs.hpp>
//looks like we might have to cut these to avoid having to deal trig for now
// void applyXRotation();

// void applyYRotation();

// void applyZRotation();

namespace renderer {

std::array<int, 16> applyTranslation(std::array<float, 16> const& transform, renderer::vec4 const& translation);

std::array<int, 16> applyRotation(std::array<float, 16> const& transform, renderer::vec4 const& direction);

std::array<int, 16> applyScaling(std::array<float, 16> const& transform, float const &scalar);

std::array<int, 16> applyScaling(std::array<float, 16> const& transform, float const &scalarX, float const &scalarY, float const &scalarZ);

renderer::vec4 normalisedVector(renderer::vec4 const& v);

std::array<float, 16> getCameraTransform(renderer::vec4 const& cameraPos);

renderer::AlignedVec4 localToTransform(std::array<float, 16> const& transform);

renderer::AlignedVec4 worldToCameraTransform(std::array<float, 16> const& transform);

//renderer::AlignedVec3 cameraToRasterTransform();

//plan is to first convert eveything to raster space and to a z prepass
}
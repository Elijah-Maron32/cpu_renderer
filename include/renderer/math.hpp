#pragma once
#include <renderer/typeDefs.hpp>
//looks like we might have to cut these to avoid having to deal trig for now
// void applyXRotation();

// void applyYRotation();

// void applyZRotation();

namespace renderer {

std::array<float, 16> applyTranslation(std::array<float, 16> const& transform, renderer::vec4 const& translation);

std::array<float, 16> applyRotationX(std::array<float, 16> const& transform, float angle);

std::array<float, 16> applyRotationY(std::array<float, 16> const& transform, float angle);

std::array<float, 16> applyRotationZ(std::array<float, 16> const& transform, float angle);

std::array<float, 16> applyRotation(std::array<float, 16> const& transform, renderer::vec4 const& direction, renderer::vec4 const& axis);

std::array<float, 16> applyScaling(std::array<float, 16> const& transform, float const &scalar);

std::array<float, 16> applyScaling(std::array<float, 16> const& transform, float const &scalarX, float const &scalarY, float const &scalarZ);

renderer::vec4 normalisedVector(renderer::vec4 const& v);

//std::array<float, 16> getCameraTransform(renderer::vec4 const& cameraPos, renderer::vec4 const& direction, float const fov, float const near, float const far);

renderer::AlignedVec4 localToTransform(std::array<float, 16> const& transform, renderer::AlignedVec4 const& verticies);

renderer::AlignedVec4 getProjectedCoordinates(AlignedVec4 const& verticies, vec4 const& cameraPos, float const fov, float const near, float const far, float const aspectRatio);

renderer::AlignedVec3 getRasterCoords(AlignedVec4 const& verticies, int height, int width);

//renderer::AlignedVec3 cameraToRasterTransform();

//plan is to first convert eveything to raster space and to a z prepass
}
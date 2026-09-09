#pragma once
#include <renderer/typeDefs.hpp>

/*
    A minimal math library that just has exactly the math used in the rasterizer. mostly just here so the code isn't inline in the rasterizer
*/
namespace renderer {

//Adds a translation to a transform matrix
std::array<float, 16> applyTranslation(std::array<float, 16> const& transform, renderer::vec4 const translation);

//Adds an x axis rotation to a transform matrix
std::array<float, 16> applyRotationX(std::array<float, 16> const& transform, float angle);

//Adds an Y axis rotation to a transform matrix
std::array<float, 16> applyRotationY(std::array<float, 16> const& transform, float angle);

//Adds an Y axis rotation to a transform matrix
std::array<float, 16> applyRotationZ(std::array<float, 16> const& transform, float angle);

//Adds a uniform scaling to a transform matrix
std::array<float, 16> applyScaling(std::array<float, 16> const& transform, float const scalar);

//Adds a non uniform scaling to a transfrom matrix
std::array<float, 16> applyScaling(std::array<float, 16> const& transform, float const scalarX, float const scalarY, float const scalarZ);

//Takes in a vector and outputs a normalised copy
renderer::vec4 normalisedVector(renderer::vec4 const v);

//Applies a transform matrix to an entire model
renderer::AlignedVec4 localToTransform(std::array<float, 16> const& transform, renderer::AlignedVec4 const& verticies);

//Transforms world space vertex buffer to NDC
renderer::AlignedVec4 getProjectedCoordinates(AlignedVec4 const& verticies, vec4 const cameraPos, float const fov, float const near, float const far, float const aspectRatio);

//Converts NDC to Raster coordinates
renderer::AlignedVec4 getRasterCoords(AlignedVec4 const& verticies, int height, int width);

}
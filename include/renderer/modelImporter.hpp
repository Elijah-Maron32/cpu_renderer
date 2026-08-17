#pragma once
#include <renderer/typeDefs.hpp>

namespace renderer{

void parseOBJFile(char *const file, renderer::AlignedVec4& verticies, renderer::AlignedFaces& faces, renderer::AlignedVec2& uvs);

void parsePNG(char *const file, std::vector<uint32_t>& output);


}
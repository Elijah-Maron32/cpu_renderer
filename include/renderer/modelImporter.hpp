#pragma once
#include <renderer/typeDefs.hpp>

namespace renderer{

//reads an obj file and stores it in the given vectors
void parseOBJFile(char *const file, renderer::AlignedVec4& verticies, renderer::AlignedFaces& faces, renderer::AlignedVec2& uvs);

//parses a png and puts it in the given vector
void parsePNG(char *const file, std::vector<uint32_t>& output);


}
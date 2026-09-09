#include <renderer/modelImporter.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace renderer {

    void parseOBJFile(char *const file, renderer::AlignedVec4& verticies, renderer::AlignedFaces& faces, renderer::AlignedVec2& uvs) {
        std::ifstream ifs(file);
        std::string line;

        while (std::getline(ifs, line)) {
            std::istringstream stream(line);
            std::string type;
            stream >> type;
            if (type == "v") {
                renderer::vec4 v;
                stream >> v.x >> v.y >> v.z;
                v.w = 1.0;
                verticies.push_back(v);
            } else if (type == "vt") { 
                renderer::vec2 vt;
                stream >> vt['u'] >> vt['v'];
                uvs.push_back(vt);
            } else if (type == "f") {
                renderer::Face f;
                for(int i=0;i<3;++i){
                    std::string tuple;
                    stream >> tuple;
                    std::istringstream tupStream(tuple);
                    std::string part;

                    std::getline(tupStream, part, '/');
                    assert(!part.empty());
                    f.Verts[i] = std::stoi(part) - 1;

                    if (std::getline(tupStream, part, '/') && !part.empty()) {
                        f.UVs[i] = std::stoi(part) - 1;
                    }
                }
                faces.push_back(f);
            }
        }
    }

    //consider changing to a template for array size
    void parsePNG(char *const file, std::vector<uint32_t>& output) {
        int x,y,n;
        unsigned char *data = stbi_load(file, &x, &y, &n, 4);
        output.assign(reinterpret_cast<uint32_t *>(data), reinterpret_cast<uint32_t *>(data) + (x*y));
        stbi_image_free(data);
        data = nullptr;
    }

}
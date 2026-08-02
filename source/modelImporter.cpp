#include <renderer/modelImporter.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

namespace renderer {

    void parseOBJFile(char *const file, renderer::AlignedVec4& verticies, renderer::AlignedFaces& faces) {
        std::ifstream ifs(file);
        std::string line;

        while (std::getline(ifs, line)) {
            std::istringstream stream(line);
            std::string type;
            stream >> type;
            if (type == "v") {
                renderer::vec4 v;
                stream >> v.x >> v.y >> v.z;
                v.w = 0.0;
                verticies.push_back(v);
            } else if (type == "f") {
                renderer::Face f;
                stream >> f[0] >> f[1] >> f[3];
                faces.push_back(f);
            }
        }
    }

}
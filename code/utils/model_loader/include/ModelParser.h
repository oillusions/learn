#pragma once
#include <VertexLayout.hpp>

class ModelParser {
    public:
        ~ModelParser() = default;
        static VertexLayout<float> ObjModelLoader(const std::string& source);
    private:
        class ObjModelLoader {
            public:
                ~ObjModelLoader() = default;
                static VertexLayout<float> parser(const std::string& source);
                static void lineProcess(const std::string &line, std::vector<float>& vertices, std::vector<float>& texCoord, std::vector<float>& normal, std::
                                        string &indexStr);
        };
};
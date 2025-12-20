#pragma once
#include <VertexLayout.hpp>

class ModelParser {
    public:
        ~ModelParser() = default;
        static std::map<std::string, VertexLayout<float>> ObjModelLoader(const std::string& source);
    private:
        class ObjModelLoader {
            public:
                ~ObjModelLoader() = default;
                static std::map<std::string, VertexLayout<float>> parser(const std::string& source);
                static void VertexProcess(const std::string& name, std::istringstream &source, std::map<std::string, VertexLayout<float>>& models);
                static void lineProcess(const std::string &line, std::vector<float>& vertices, std::vector<float>& texCoord, std::vector<float>& normal, std::
                                        string &indexStr);
        };
};
#include "ModelParser.h"
#include <sstream>

using namespace std;

VertexLayout<float> ModelParser::ObjModelLoader(const std::string &source) {
    if (source.empty()) {
        throw runtime_error("错误: obj模型加载器源为空");
    }
    return ObjModelLoader::parser(source);
}

VertexLayout<float> ModelParser::ObjModelLoader::parser(const std::string &source) {
    VertexLayout<float>::LayoutBuilder builder{};
    vector<float> vertices{};
    vector<float> texCoord{};
    vector<float> normal{};
    string indexStr{};
    istringstream sourceStrStream(source);
    string line;
    while (getline(sourceStrStream, line)) {
        if (line[0] == '#' || line.empty()) continue;
        lineProcess(line, vertices, texCoord, normal, indexStr);
    }
    if (!vertices.empty()) {
        builder.appendElement("vertices", 3);
        builder.additionalSource("vertices", move(vertices));
    }
    if (!texCoord.empty()) {
        builder.appendElement("texCoord", 2);
        builder.additionalSource("texCoord", move(texCoord));
    }
    if (!normal.empty()) {
        builder.appendElement("normal", 3);
        builder.additionalSource("normal", move(normal));
    }
    if (!indexStr.empty()) {
        builder.indices(move(indexStr));
    }
    return builder.build();
}

void ModelParser::ObjModelLoader::lineProcess(const std::string &line, vector<float>& vertices, vector<float>& texCoord, vector<float>& normal, string &indexStr) {
    istringstream lineStrStream(line);
    string token;
    while (getline(lineStrStream, token, ' ')) {
        if (token == "v") {  // 顶点坐标
            float x, y, z;
            lineStrStream >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if (token == "vt") {  // 纹理坐标
            float u, v;
            lineStrStream >> u >> v;
            texCoord.push_back(u);
            texCoord.push_back(v);
        }
        else if (token == "vn") {  // 法线
            float x, y, z;
            lineStrStream >> x >> y >> z;
            normal.push_back(x);
            normal.push_back(y);
            normal.push_back(z);
        }
        else if (token == "f") {
            while (getline(lineStrStream, token, ' ')) {
                indexStr += line.substr(2, line.length());
                indexStr += " ";
                break;
            }
        }
    }
}




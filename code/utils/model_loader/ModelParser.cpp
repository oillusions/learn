#include "ModelParser.h"
#include <sstream>

using namespace std;

std::map<std::string, VertexLayout<float> > ModelParser::ObjModelLoader(const std::string &source) {
    if (source.empty()) {
        throw runtime_error("错误: obj模型加载器源为空");
    }
    return ObjModelLoader::parser(source);
}

std::map<std::string, VertexLayout<float> > ModelParser::ObjModelLoader::parser(const std::string &source) {
    map<string, VertexLayout<float>> models{};
    istringstream sourceStrStream(source);
    string line;
    while (getline(sourceStrStream, line)) {
        if (line[0] == '#' || line.empty()) continue;
        if (line[0] == 'o') {
            VertexProcess(line.substr(2, line.back()), sourceStrStream, models);
        }
    }
    return models;
}

void ModelParser::ObjModelLoader::VertexProcess(const string& name, istringstream &source, std::map<std::string, VertexLayout<float>>& models) {
    auto builder = VertexLayout<float>::builder();
    vector<float> vertices{};
    vector<float> texCoord{};
    vector<float> normal{};
    string indexStr;
    string line;
    while (getline(source, line)) {
        if (line[0] == '#' || line.empty()) continue;
        if (line[0] == 'o') {
            VertexProcess(line.substr(2, line.back()), source, models);
        }
        lineProcess(line, vertices, texCoord, normal, indexStr);
    }
    cout << "加载模型 " << name << ": " << endl;
    if (!vertices.empty()) {
        builder.appendElement("vertices", 3)
            .additionalSource("vertices", std::move(vertices));
        cout << name << ": 添加Vertices" << endl;
    }
    if (!texCoord.empty()) {
        builder.appendElement("texCoord", 2)
            .additionalSource("texCoord", std::move(texCoord));
        cout << name << ": 添加TexCoord" << endl;
    }
    if (!normal.empty()) {
        builder.appendElement("normal", 3)
            .additionalSource("normal", std::move(normal));
        cout << name << ": 添加Normal" << endl;
    }
    if (!indexStr.empty()) {
        builder.indices(std::move(indexStr));
        cout << name << ": 添加Indices" << endl;
    }
    models.emplace(name, std::move(builder.build()));
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
        else if (token == "f") {  // 面
            while (getline(lineStrStream, token, ' ')) {
                indexStr += line.substr(2, line.length());
                indexStr += " ";
                break;
            }
        }
    }
}




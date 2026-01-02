#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>

#include <stb_image.h>

#include <GlobalLogger.hpp>
#include <RAIIWrapper.hpp>
#include <Resource.hpp>

namespace resource::utils {
    using namespace std;
    namespace fs = std::filesystem;

    inline string readFileToStr(const fs::path& path) {
        if (!fs::exists(path)) {
            glog.log<DefaultLevel::Error>("文件不存在: " + path.string());
            return {};
        }

        ifstream file(path);
        if (!file.is_open()) {
            glog.log<DefaultLevel::Error>("文件无法打开: " + path.string());
            return {};
        }
        string out{istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
        file.close();
        return out;
    }
}

class Texture: public RAIIWrapper<GLuint>, IResource {
    public:
        explicit Texture(const std::filesystem::path& path) {
            uint8_t* data = stbi_load(path.string().c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

            if (data == nullptr) {
                glog.log<DefaultLevel::Error>("错误: 纹理文件无法加载: " + path.string());
                throw;
            }
            glGenTextures(1, &_value);
            glBindTexture(GL_TEXTURE_2D, _value);
            glActiveTexture(GL_TEXTURE0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            _value = 1;
            stbi_image_free(data);
        }
        ~Texture() override {
            glog.log<DefaultLevel::Debug>("纹理对象已析构: " + std::to_string(_value));
            glDeleteTextures(1, &_value);
        }
    private:
        int imgWidth{}, imgHeight{};
        int nrChannels{};
};

class VertexArrays: public RAIIWrapper<GLuint> {
    public:
        explicit VertexArrays(const GLuint value = 0) {
            _value = value;
        }
        ~VertexArrays() override {
            glog.log<DefaultLevel::Debug>("顶点数组对象已析构: " + std::to_string(_value));
            glDeleteVertexArrays(1, &_value);
        }
};

class BufferObject: public RAIIWrapper<GLuint> {
    public:
        explicit BufferObject(const GLuint value = 0) {
            _value = value;
        }

        ~BufferObject() override {
            glDeleteBuffers(1, &_value);
        }
};

inline AnyResourceManager arm{};
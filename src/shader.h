
#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    ~Shader();

    void use() const;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec3(const std::string &name, const glm::vec3 &vec) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    unsigned int ID;

    static void checkCompileErrors(unsigned int shader, const std::string &type);
};


#endif

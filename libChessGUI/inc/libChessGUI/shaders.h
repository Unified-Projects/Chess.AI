#pragma once
#include <libChessGUI/configure.h>

namespace Shaders
{
    extern const char* VertexShaderText;
    extern const char* FragmentShaderText;

    // Shader creation
    unsigned int compileShader(unsigned int type, const char* source);
    void createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

    // Global Shader Access
    extern const unsigned int& ShaderProgram;

    // Shader interactions
    void Toggle();
    inline void SendInt(int& ref, const char* location) {glUniform1i(glGetUniformLocation(ShaderProgram, location), ref);}
    inline void SendVec3(glm::vec3& ref, const char* location) {glUniform3fv(glGetUniformLocation(ShaderProgram, location), 1, glm::value_ptr(ref));}
    inline void SendVec4(glm::vec4& ref, const char* location) {glUniform4fv(glGetUniformLocation(ShaderProgram, location), 1, glm::value_ptr(ref));}

    // Generic values
    extern glm::vec3 ZeroVec3;
    extern glm::vec4 ZeroVec4;
    extern int falseInt;
    extern int trueInt;

} // namespace GUI

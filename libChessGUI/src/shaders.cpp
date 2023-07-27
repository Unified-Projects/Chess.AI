#include <libChessGUI/shaders.h>
#include <libChessGUI/configure.h>

#include <iostream>

const char* Shaders::VertexShaderText = "#version 460 core\n"
    "layout (location = 0) in vec3 vertex_position;\n"
    "layout (location = 1) in vec4 vertex_color;\n"
    "layout (location = 2) in vec2 vertex_texcoord;\n"
    "out vec3 vs_position;\n"
    "out vec4 vs_color;\n"
    "out vec2 vs_texcoord;\n"
    "uniform vec3 Offset;\n"
    "void main()\n"
    "{\n"
    "   vs_position = vertex_position;\n"
    "   vs_color = vertex_color;\n"
    "   vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);\n"
    "   gl_Position = vec4(vertex_position + Offset, 1.0);\n"
    "}\0";

const char* Shaders::FragmentShaderText = "#version 460 core\n"
    "in vec3 vs_position;\n"
    "in vec4 vs_color;\n"
    "in vec2 vs_texcoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D Texture;\n"
    "uniform int UsingTexture;\n"
    "uniform vec4 ColourOffset;\n"
    "void main()\n"
    "{\n"
    "   if(UsingTexture == 1){\n"
    "       if (texture(Texture, vs_texcoord).xyzw == vec4(0.f)){\n"
    "           discard;\n"
    "       }\n"
    "       FragColor  = texture(Texture, vs_texcoord);\n"
    "   }\n"
    "   else{\n"
    "       if (vs_color.xyzw == vec4(0.f)){\n"
    "           discard;\n"
    "       }\n"
    "       FragColor = vs_color + ColourOffset;\n"
    "   }\n"
    "}\0";

// Shader program store
static unsigned int ShaderProgramAccess;
const unsigned int& Shaders::ShaderProgram = ShaderProgramAccess;

glm::vec3 Shaders::ZeroVec3 = glm::vec3(0.f);
glm::vec4 Shaders::ZeroVec4 = glm::vec4(0.f);
int Shaders::falseInt = 0;
int Shaders::trueInt = 1;

// Shader section compiling
unsigned int Shaders::compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw std::runtime_error("Shader compilation failed:\n" + std::string(infoLog));
    }

    return shader;
}

// Main program creation
void Shaders::createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        throw std::runtime_error("Shader program linking failed:\n" + std::string(infoLog));
    }

    ShaderProgramAccess = program;
}

// Toggling usage
static bool Toggled = false;
void Shaders::Toggle(){
    if (Toggled){
        glUseProgram(0);
    }else{
        glUseProgram(ShaderProgram);
    }

    Toggled = !Toggled;
}
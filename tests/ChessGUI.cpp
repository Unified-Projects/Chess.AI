#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <chess/board.h>

#include <iostream>
#include <vector>

// GLOBAL WINDOW
#define SIZE 640

int Scale = 8;
float Dimention = 1.0f / Scale;
float XZero = -1.f + Dimention;
float YZero = -1.f + Dimention;

const char* VertexShader = "#version 460 core\n"
    "layout (location = 0) in vec3 vertex_position;\n"
    "layout (location = 1) in vec3 vertex_color;\n"
    "layout (location = 2) in vec2 vertex_texcoord;\n"
    "out vec3 vs_position;\n"
    "out vec3 vs_color;\n"
    "out vec2 vs_texcoord;\n"
    "void main()\n"
    "{\n"
    "   vs_position = vertex_position;\n"
	"   vs_color = vertex_color;\n"
	"   vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);\n"
    "   gl_Position = vec4(vertex_position, 1.0);\n"
    "}\0";

const char* FragmentShader = "#version 460 core\n"
    "in vec3 vs_position;\n"
    "in vec3 vs_color;\n"
    "in vec2 vs_texcoord;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor  = vec4(vs_color, 1.0f);\n"
    "}\0";

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

struct Vertex
{
    //Values for rendering
    glm::vec3 position = {0, 0, 0};
    glm::vec3 color = {1, 1, 1};
    glm::vec2 uv = {0, 0};
};

struct Square{
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 Colour = glm::vec3(235.f/255.f, 210.f/255.f, 184.f/255.f);

    void GenerateMesh(){
        //Test if already generated and if so clear all the buffers
        if (this->VAO) {
            glDeleteBuffers(1, &this->EBO);
            glDeleteBuffers(1, &this->VAO);
            glDeleteBuffers(1, &this->VBO);
        }

        glGenVertexArrays(1, &VAO);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Bind and copy vertex data
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(VERTS), VERTS, GL_STATIC_DRAW);

        // Bind and copy index data
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        // glEnableVertexAttribArray(0);  

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    Square(int x, int y){ 
        if((x + y) % 2 == 0){
            Colour = glm::vec3(161.f/255.f, 111.f/255.f, 90.f/255.f);
        }

        VBO = 0;
        VAO = 0;
        EBO = 0;

        vertices = {
            {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + -Dimention, 0.0f), Colour, glm::vec2(0, 1)},
            {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + -Dimention, 0.0f), Colour, glm::vec2(0, 0)},
            {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + Dimention, 0.0f), Colour, glm::vec2(1, 0)},
            {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + Dimention, 0.0f) , Colour, glm::vec2(1, 1)}
        };

        indices = {
            0, 1, 2,    // First triangle
            2, 3, 0     // Second triangle
        };

        GenerateMesh();
    }

    void Render(){
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    }
};

// Board
std::vector<Square> GraphicBoard = {};
Board board;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void processInput(GLFWwindow *window);

// Enable
void OpenGLEnable(GLenum ToEnable) {
    glEnable(ToEnable);
}

//Disable
void OpenGLDisable(GLenum ToDisable) {
    glEnable(ToDisable);
}

//PolygonMode
void OpenGLPolygonMode(GLenum Face, GLenum Mode) {
    glPolygonMode(Face, Mode);
}

//Blending
void OpenGLBlend(GLenum SFactor, GLenum DFactor) {
    glBlendFunc(SFactor, DFactor);
}

//GLRendering
void OpenGLRendering(GLenum Cull, GLenum Front) {
    glCullFace(Cull);
    glFrontFace(Front);
}

unsigned int compileShader(unsigned int type, const char* source) {
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

unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
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

    return program;
}

int main()
{
    // GLFW Main intialiser
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the window and attach it
    GLFWwindow* window = glfwCreateWindow(SIZE, SIZE, "Chess", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD Main initialiser
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    // Window configures
    glViewport(0, 0, SIZE, SIZE);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    //Configure
    // OpenGLRendering(GL_BACK, GL_CCW);
    OpenGLEnable(GL_BLEND);
    OpenGLBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // OpenGLPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Shaders
    vertexShader = compileShader(GL_VERTEX_SHADER, VertexShader);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, FragmentShader);
    shaderProgram = createShaderProgram(vertexShader, fragmentShader);

    { // Free ups
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Generate board
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            GraphicBoard.push_back(Square(x, y));
        } 
    } 

    // GraphicBoard.push_back(Square(0, 0));

    // Main window loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render board
        for(Square s : GraphicBoard){
            s.Render();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
  
    // Kill all
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
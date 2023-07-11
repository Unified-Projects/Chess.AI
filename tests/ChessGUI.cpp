#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <SOIL2/SOIL2.h>

#include <chess/board.h>

#include <iostream>
#include <vector>
#include <string>

// GLOBAL WINDOW
#define SIZE 640

int Scale = 8;
float Dimention = 1.0f / Scale;
float XZero = -1.f + Dimention;
float YZero = -1.f + Dimention;

const char* VertexShader = "#version 460 core\n"
    "layout (location = 0) in vec3 vertex_position;\n"
    "layout (location = 1) in vec4 vertex_color;\n"
    "layout (location = 2) in vec2 vertex_texcoord;\n"
    "out vec3 vs_position;\n"
    "out vec4 vs_color;\n"
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
    "in vec4 vs_color;\n"
    "in vec2 vs_texcoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D Texture;\n"
    "uniform int UsingTexture;\n"
    "void main()\n"
    "{\n"
    "   if(UsingTexture == 1){\n"
    "       FragColor  = texture(Texture, vs_texcoord);\n"
    "   }\n"
    "   else{\n"
    "       FragColor  = vs_color;\n"
    "   }\n"
    "}\0";

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

struct Vertex
{
    //Values for rendering
    glm::vec3 position = {0, 0, 0};
    glm::vec4 color = {1, 1, 1, 1};
    glm::vec2 uv = {0, 0};
};

struct PieceTexture{
    std::string FilePath;
    uint8_t* Data;

    int width;
    int height;
    unsigned int TextureID;

    PieceTexture(const char* filepath){
        this->FilePath = filepath;

        // Read data
        this->Data = SOIL_load_image(filepath, &this->width, &this->height, NULL, SOIL_LOAD_RGBA);

        // Load the image to GLFW
        glGenTextures(1, &TextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        // Free Data
        SOIL_free_image_data(this->Data);
    }

    void Bind(){
        glBindTexture(GL_TEXTURE_2D, this->TextureID);
        glActiveTexture(GL_TEXTURE0 + this->TextureID);

        glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), TextureID);
    }
};

struct Square{
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::vec4 Colour = glm::vec4(235.f/255.f, 210.f/255.f, 184.f/255.f, 1.f);

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

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    Square(int x, int y){ 
        if((x + y) % 2 == 0){
            Colour = glm::vec4(161.f/255.f, 111.f/255.f, 90.f/255.f, 1.f);
        }

        VBO = 0;
        VAO = 0;
        EBO = 0;

        vertices = {
            {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + -Dimention, 0.0f), Colour, glm::vec2(0, 0)},
            {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + -Dimention, 0.0f), Colour, glm::vec2(1, 0)},
            {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + Dimention, 0.0f), Colour, glm::vec2(1, 1)},
            {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + Dimention, 0.0f) , Colour, glm::vec2(0, 1)}
        };

        indices = {
            0, 1, 2,    // First triangle
            2, 3, 0     // Second triangle
        };

        GenerateMesh();
    }

    void Render(){
        glUseProgram(shaderProgram);

        // Tell the shader we are not using a texture
        glUniform1i(glGetUniformLocation(shaderProgram, "UsingTexture"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    }
};

struct PieceSquare{
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    PieceTexture* tex;

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

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    PieceSquare(int x, int y, PieceTexture* Texture){ 
        tex = Texture;

        VBO = 0;
        VAO = 0;
        EBO = 0;

        vertices = {
            {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + -Dimention, 0.0f), glm::vec4(0.f), glm::vec2(0, 0)},
            {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + -Dimention, 0.0f), glm::vec4(0.f), glm::vec2(1, 0)},
            {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + Dimention, 0.0f), glm::vec4(0.f), glm::vec2(1, 1)},
            {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + Dimention, 0.0f) , glm::vec4(0.f), glm::vec2(0, 1)}
        };

        indices = {
            0, 1, 2,    // First triangle
            2, 3, 0     // Second triangle
        };

        GenerateMesh();
    }

    void Render(){
        bool noBind = false;

        glUseProgram(shaderProgram);

        // Activate the texture
        if(tex)
            tex->Bind();
        else
            noBind = true;

        if(!noBind){
            // Tell the shader we are using a texture
            glUniform1i(glGetUniformLocation(shaderProgram, "UsingTexture"), 1);
        }
        else
            // Tell the shader we are not using a texture
            glUniform1i(glGetUniformLocation(shaderProgram, "UsingTexture"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    }
};

struct PieceTextureMapper{
    std::vector<PieceTexture> Pieces;

    PieceTextureMapper(){
        // TODO: ISSUE :( not independent and inconsistent with other systems
        // Load White Textures
        Pieces.push_back(PieceTexture("../../icons/wb.png"));
        Pieces.push_back(PieceTexture("../../icons/wk.png"));
        Pieces.push_back(PieceTexture("../../icons/wn.png"));
        Pieces.push_back(PieceTexture("../../icons/wp.png"));
        Pieces.push_back(PieceTexture("../../icons/wq.png"));
        Pieces.push_back(PieceTexture("../../icons/wr.png"));

        // Load Black Textures
        Pieces.push_back(PieceTexture("../../icons/bb.png"));
        Pieces.push_back(PieceTexture("../../icons/bk.png"));
        Pieces.push_back(PieceTexture("../../icons/bn.png"));
        Pieces.push_back(PieceTexture("../../icons/bp.png"));
        Pieces.push_back(PieceTexture("../../icons/bq.png"));
        Pieces.push_back(PieceTexture("../../icons/br.png"));
    }

    PieceTexture* GetTex(Type pT, Colour pC){
        int Offsets = (pC == WHITE) ? 0 : 6;

        if(pT == NULL_TYPE || pC == NULL_COLOUR){
            return nullptr;
        }

        switch (pT)
        {
        case PAWN:
            return &Pieces.at(Offsets+3);
        case ROOK:
            return &Pieces.at(Offsets+5);
        case KNIGHT:
            return &Pieces.at(Offsets+2);
        case BISHOP:
            return &Pieces.at(Offsets+0);
        case KING:
            return &Pieces.at(Offsets+1);
        case QUEEN:
            return &Pieces.at(Offsets+4);
        
        default:
            return nullptr;
        }
    }
};

// Board
Board board;

struct BoardGUI{
    std::vector<Square> BoardSquares;
    std::vector<PieceSquare> BoardPieces;

    PieceTextureMapper Mapper;

    BoardGUI(){
        // Load all the textures manually

        for(int y = 0; y < 8; y++){
            for(int x = 0; x < 8; x++){
                BoardSquares.push_back(Square(x, y));
                BoardPieces.push_back(PieceSquare(x, y, nullptr));
            }
        }

        // Load icons
        for(Piece* p : board.board){
            BoardPieces.at(p->Square).tex = Mapper.GetTex(p->GetT(), p->GetC());
        }
    }

    void Render(){
        for(Square s : BoardSquares){
            s.Render();
        }
        for(PieceSquare p : BoardPieces){
            p.Render();
        }
    }

    void Update(){
        // Update icons
        for(Piece* p : board.board){
            // BoardPieces.at(p->Square).tex = Mapper.GetTex(p->GetT(), p->GetC());
        }
    }
};


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
    board.InitBoard();
    BoardGUI BGui;

    // GraphicBoard.push_back(Square(0, 0));

    // Main window loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render board
        BGui.Render();

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
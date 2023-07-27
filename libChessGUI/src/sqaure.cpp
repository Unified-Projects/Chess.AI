#include <libChessGUI/square.h>
#include <libChessGUI/shaders.h>
#include <libChessGUI/textures.h>

using namespace GUI;

// Meshing
void Square::GenerateMesh(){
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

Square::Square(int x, int y, float z, glm::vec4 colour){
    VBO = 0;
    VAO = 0;
    EBO = 0;

    this->SquareColour = colour;

    vertices = {
        {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + -Dimention, z), colour, glm::vec2(0, 0)},
        {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + -Dimention, z), colour, glm::vec2(1, 0)},
        {glm::vec3(XZero + (x * Dimention * 2) + Dimention, YZero + (y * Dimention * 2) + Dimention, z), colour, glm::vec2(1, 1)},
        {glm::vec3(XZero + (x * Dimention * 2) + -Dimention, YZero + (y * Dimention * 2) + Dimention, z) , colour, glm::vec2(0, 1)}
    };

    indices = {
        0, 1, 2,    // First triangle
        2, 3, 0     // Second triangle
    };

    GenerateMesh();
}

// Generics
int GenShaderTex = 1;
int GenShaderNoTex = 0;
glm::vec3 GenShaderOffst = glm::vec3(0.f);
void Square::Render(){
    // Tell the shader we are not using a texture
    Shaders::Toggle();
    Shaders::SendInt(GenShaderNoTex, "UsingTexture");
    Shaders::SendVec3(GenShaderOffst, "Offset");
    Shaders::SendVec4(ColourOffset, "ColourOffset");

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    Shaders::Toggle();
}

BoardSquare::BoardSquare(int x, int y)
    :   Square(x, y, 0.f, ((x + y) % 2 == 0) ? glm::vec4(161.f/255.f, 111.f/255.f, 90.f/255.f, 1.f) : glm::vec4(235.f/255.f, 210.f/255.f, 184.f/255.f, 1.f))
{
    Toggled = false;
}

void BoardSquare::HighlightToggle(){
    if(!Toggled){
        if(SquareColour == glm::vec4(161.f/255.f, 111.f/255.f, 90.f/255.f, 1.f))
            ColourOffset = glm::vec4(0.25f, 0.f, 0.f, 0.f);
        else
            ColourOffset = glm::vec4(0.25f, -0.25f, -.25f, 0.f);
    }
    else{
        ColourOffset = glm::vec4(0.f);
    }
}

PieceSquare::PieceSquare(int x, int y, PieceTexture* Texture)
    : Square(x, y, -0.50f)
{
    tex = Texture;
    Offset = glm::vec3(0.f);
}

void PieceSquare::Render(){
    bool noBind = false;

    // Activate the texture
    if(tex)
        tex->Bind();
    else
        noBind = true;

    if(!noBind){
        // Tell the shader we are using a texture
        Shaders::Toggle();
        Shaders::SendInt(GenShaderTex, "UsingTexture");
        Shaders::SendVec3(Offset, "Offset");
        Shaders::SendVec4(ColourOffset, "ColourOffset");
    }
    else{
        // Tell the shader we are not using a texture
        Shaders::Toggle();
        Shaders::SendInt(GenShaderNoTex, "UsingTexture");
        Shaders::SendVec3(Offset, "Offset");
        Shaders::SendVec4(ColourOffset, "ColourOffset");
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    Shaders::Toggle(); // Unbind
}
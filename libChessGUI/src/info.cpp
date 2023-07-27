#include <libChessGUI/info.h>
#include <libChessGUI/shaders.h>
#include <libChessGUI/textures.h>
#include <libChessGUI/gui.h>
#include <chess/board.h>

using namespace GUI;

bool InformationScreen::AddWidget(InformationWidget* Widget){
    if(CurrentPos + (Widget->Size * 2) > 1.f){
        return false; // No Fitty
    }

    // Now add the widget
    this->Widgets.push_back(Widget);
    this->CurrentPos += Widget->Size * 2;

    // Call widgets initialiser and set constraints
    Widget->AssignSizeConstaints(0.5f, CurrentPos-Widget->Size * 2, CurrentPos);
    Widget->Initialise();

    return true;
}

void InformationScreen::Update(){
    for(InformationWidget* w : this->Widgets){
        w->Update();
    }
}

void InformationScreen::Render(){
    for(InformationWidget* w : this->Widgets){
        w->Render();
    }
}

void MoveInfoWidget::GenerateMesh(){
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

void MoveInfoWidget::Initialise(){
    VBO = 0;
    VAO = 0;
    EBO = 0;

    glm::vec4 colour = glm::vec4(0.f);

    vertices = {
        {glm::vec3(StartX, StartY, 0), colour, glm::vec2(0, 0)},
        {glm::vec3(1, StartY, 0), colour, glm::vec2(1, 0)},
        {glm::vec3(1, EndY, 0), colour, glm::vec2(1, 1)},
        {glm::vec3(StartX, EndY, 0) , colour, glm::vec2(0, 1)}
    };

    indices = {
        0, 1, 2,    // First triangle
        2, 3, 0     // Second triangle
    };

    GenerateMesh();
}

MoveInfoWidget::MoveInfoWidget()
    : InformationWidget((1.f/3.f))
{
    // Nothing blocky setup due to not knowing constraints
    tex = nullptr;
}

void MoveInfoWidget::Update(){
    if(!GlobWin){ // Unlikely error but just in case
        return;
    }

    // Get the current mode
    if(!GlobWin->GetHost()){
        return; // No host to get
    }

    Colour mover = GlobWin->GetHost()->CurrentMove;

    // Get new texture
    tex = GlobWin->Mapper->GetTex(PAWN, mover);
    // tex = GlobWin->Mapper->GetTex(PAWN, WHITE);
}

void MoveInfoWidget::Render(){
    bool noBind = false;

    // Activate the texture
    if(tex)
        tex->Bind();
    else
        noBind = true;

    if(!noBind){
        // Tell the shader we are using a texture
        Shaders::Toggle();
        Shaders::SendInt(Shaders::trueInt, "UsingTexture");
        Shaders::SendVec3(Shaders::ZeroVec3, "Offset");
        Shaders::SendVec4(Shaders::ZeroVec4, "ColourOffset");
    }
    else{
        // Tell the shader we are not using a texture
        Shaders::Toggle();
        Shaders::SendInt(Shaders::falseInt, "UsingTexture");
        Shaders::SendVec3(Shaders::ZeroVec3, "Offset");
        Shaders::SendVec4(Shaders::ZeroVec4, "ColourOffset");
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    Shaders::Toggle(); // Unbind
}

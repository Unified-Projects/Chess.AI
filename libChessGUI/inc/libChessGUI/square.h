#pragma once

#include <libChessGUI/configure.h>
#include <vector>

namespace GUI
{
    // Texture stuff
    class PieceTexture;

    struct Vertex
    {
        //Values for rendering
        glm::vec3 position = {0, 0, 0};
        glm::vec4 color = {1, 1, 1, 1};
        glm::vec2 uv = {0, 0};
    };

    class Square{
    protected: // Object Information
        unsigned int VBO;
        unsigned int VAO;
        unsigned int EBO;

        // Meshing
        void GenerateMesh();

    public: // Information
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        glm::vec4 SquareColour = glm::vec4(235.f/255.f, 210.f/255.f, 184.f/255.f, 1.f);
        glm::vec4 ColourOffset = glm::vec4(0.f);

        int SquarePosition;
        int X;
        int Y;

        Square(int x, int y, float z, glm::vec4 colour = glm::vec4(0.f));
        ~Square(
            /* TODO:
            Delete VBO, VAO, EBO
            */
        ) {}

        void Update(); // Unused
        virtual void Render();
    };

    class BoardSquare : public Square{
    public:
        bool Toggled;

    public:
        BoardSquare(int x, int y);

        void HighlightToggle();
    };

    class PieceSquare : public Square{
    public:
        PieceTexture* tex;
        glm::vec3 Offset;

    public:
        PieceSquare(int x, int y, PieceTexture* Texture);

        void Render();
    };
} // namespace GUI::Textures

#pragma once
#include <vector>
#include <libChessGUI/square.h>

#include <chess/piece.h>

namespace GUI
{
    // PreDefs
    class PieceTexture;
    class InformationWidget;

    class InformationScreen{
    public: // Mutators
        bool AddWidget(InformationWidget* Widget); // Retuns false on not enough room

    public:
        void Update();
        void Render();

    protected: // Varaibles
        float CurrentPos = -1.f;
        std::vector<InformationWidget*> Widgets;
    };

    class InformationWidget{
        friend InformationScreen;
    protected: // Propper Setups
        float StartX;
        float StartY;
        float EndY;

        void AssignSizeConstaints(float X, float Y, float E) {StartX = X; StartY = Y; EndY = E;return;};    

        virtual void Initialise() {};
    public:
        inline InformationWidget(float Size) {this->Size = Size;}

    public:
        virtual void Update() {return;}
        virtual void Render() {return;}
    protected:
        float Size;
    };

    class BlankWidget : public InformationWidget{
    public:
        inline BlankWidget() : InformationWidget((1.f/3.f)){

        }
    };

    class MoveInfoWidget : public InformationWidget{
    protected:
        void Initialise();

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        std::vector<Vertex> vertices;
        std::vector<int> indices;

        void GenerateMesh();

    public:
        MoveInfoWidget();

        PieceTexture* tex;

    public:
        void Update();
        void Render();
    };

    class PieceWidget : public InformationWidget{
    protected:
        void Initialise();

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        std::vector<Vertex> vertices;
        std::vector<int> indices;

        void GenerateMesh();

    public:
        PieceWidget(Type t, Colour c);

        PieceTexture* tex;

    public:
        void Update();
        void Render();
    };

    class GameButtonsWidget : public InformationWidget{

    };

    class MoveListWidget : public InformationWidget{

    };

} // namespace GUI

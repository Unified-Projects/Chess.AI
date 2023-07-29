#pragma once
#include <libChessGUI/configure.h>
#include <vector>
#include <functional>
#include <libChessGUI/info.h>
#include <chess/board.h>

namespace GUI{
    // TODO IMPLEMENT MULTI-WINDOW Compatibility

    // Pre-Defining
    class BoardSquare;
    class PieceSquare;
    class PieceTextureMapper;
    class InformationScreen;

    // Intitialiser
    GLFWwindow* IntitaliseGraphics();

    class GameWindow{
    protected: // Protected Variables
        // Attached game
        Board* host;

        // Blocker
        bool Functional = false;

        // Window
        GLFWwindow* Context;

    public: // Public
        std::vector<BoardSquare> BoardSquares;
        std::vector<PieceSquare> BoardPieces;

        PieceTextureMapper* Mapper;

        PieceSquare* RenderDelay = nullptr;

    public: // Callbacks
        std::function<bool(Move&, bool)> OnMoveWanted; // Allows user to prevent a move from happening
        std::function<void()> KeybaordCallback; // TODO: I cba rn
        std::function<void()> MouseCallback; // TODO: I cba rn

    public: // Public Interfaces

        // Constructors / De-constructors
        GameWindow();
        ~GameWindow(
            /* TODO: list
               Remove Window
               Remove shader
               Remove Mapper */
        );

        // Interfaces
        void Attach(Board* game);
        Board* GetHost() {return host;}

        // Management
        void Update();
        void Render();
        inline bool IsClosing() {return glfwWindowShouldClose(Context);}

        // FPS Limiting
        void FrameLimit(int FPS);

        // Termination
        inline void Terminate() {glfwTerminate();}

        // Checks
        inline bool IsAttached() {return( host != nullptr);}

    public: // Callbacks
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
        static void mouse_cursor_callback( GLFWwindow * window, double xpos, double ypos);
        void keyboard_processing();

        static bool DragStarted;
        static double CursorX;
        static double CursorY;
        static double PrevCursorX;
        static double PrevCursorY;
        static int CurrentHoveredSquare;

    protected: // Board Positioning and fixes
        static int DispX;
        static int DispY;

        int BoardX;
        int BoardY;
        int InfoX;

        // Multi-layer rendering profile
        GLuint Framebuffer = 0;
        GLuint Renderbuffer = 0;
        GLuint Texture = 0;

    public: // Info Window
        bool InfoActive = false;
        bool BlockInfoPress = false;
        int InfoWinSize = 3;

        InformationScreen Info;

    public: // Key Blockers
        bool UndoPressBlocker = false;
        bool UndoAllowed = true;
    };

    // Global Game Constant
    extern GameWindow* GlobWin;
};
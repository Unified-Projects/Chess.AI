#include <libChessGUI/gui.h>
#include <libChessGUI/shaders.h>
#include <libChessGUI/textures.h>
#include <libChessGUI/square.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include <chess/board.h>

#include <iostream>

using namespace GUI;

static bool SetupYet = false; // TEMPORARY MULTI-INSTANCE BLOCKER

bool GameWindow::DragStarted = false;
double GameWindow::CursorX = 0;
double GameWindow::CursorY = 0;
double GameWindow::PrevCursorX = 0;
double GameWindow::PrevCursorY = 0;
int GameWindow::CurrentHoveredSquare = 0;
int GameWindow::DispX = 0;
int GameWindow::DispY = 0;

GameWindow* GUI::GlobWin = nullptr;

std::vector<Square*> HighlightedSquares = {};

GLFWwindow* GUI::IntitaliseGraphics(){
    if(SetupYet){
        return nullptr;
    }

    // GLFW Main intialiser
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS

    // Create the window and attach it
    GLFWwindow* window = glfwCreateWindow(SIZE, SIZE, "Chess", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // GLAD Main initialiser
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }  

    // Window configures
    glViewport(0, 0, SIZE, SIZE);
    glfwSetFramebufferSizeCallback(window, GameWindow::framebuffer_size_callback); 
    glfwSetCursorPosCallback(window, GameWindow::mouse_cursor_callback);
    glfwSetMouseButtonCallback(window, GameWindow::mouse_callback);

    //Configure
    OpenGLRendering(GL_BACK, GL_CCW);
    OpenGLEnable(GL_BLEND);
    OpenGLBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    OpenGLPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    OpenGLEnable(GL_DEPTH_TEST);

    return window;
};

// Main setup
GameWindow::GameWindow(){
    if(SetupYet){ // Blocker
        Functional = false;
        return;
    }

    // Attach to global
    GlobWin = this;

    // Create the main window
    Context = IntitaliseGraphics();

    if(!Context){ // (nullptr)
        Functional = false;
        SetupYet = true; // Prevent any other errors (incase half initialised)
        return;
    }

    // Allow this window to function
    Functional = true;
    SetupYet = true;

    // Now compile main shader

    unsigned int vertexShader = Shaders::compileShader(GL_VERTEX_SHADER, Shaders::VertexShaderText);
    unsigned int fragmentShader = Shaders::compileShader(GL_FRAGMENT_SHADER, Shaders::FragmentShaderText);
    Shaders::createShaderProgram(vertexShader, fragmentShader);

    { // Free ups
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Setup textures
    Mapper = new PieceTextureMapper();

    // Create the board
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            BoardSquares.push_back(BoardSquare(x, y));
        }
    }

    // Disp Value
    DispX = SIZE;
    DispY = SIZE;

    // Board sizing
    BoardX = SIZE;
    BoardY = SIZE;
}

// Interfaces
void GameWindow::Attach(Board* game) {
    if(!Functional){
        return; // We dont want to use a non-functional instance
    }

    host = game;

    // Clear Pieces
    BoardPieces.clear();

    if(!host){
        return; // Just in case we detach!
    }

    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            BoardPieces.push_back(PieceSquare(x, y, Mapper->GetTex(host->board[(y*8)+x]->GetT(), host->board[(y*8)+x]->GetC())));
        }
    }
}

std::chrono::_V2::steady_clock::time_point next_frame;
bool timer_done = false;

// Management
void GameWindow::Update(){
    if(!Functional){
        return; // We dont want to use a non-functional instance
    }

    // Get inputs
    glfwPollEvents();

    if(!timer_done){ // Timer setup
        timer_done = true;
        next_frame = std::chrono::steady_clock::now();
    }

    // Update icons
    if(host){
        for(int y = 0; y < 8; y++){
            for(int x = 0; x < 8; x++){
                BoardPieces.at((y * 8) + x).tex = Mapper->GetTex(host->board[(y*8)+x]->GetT(), host->board[(y*8)+x]->GetC());
            }
        }
    }

    keyboard_processing();
}

void GameWindow::Render(){
    if(!Functional){
        return; // We dont want to use a non-functional instance
    }

    { // Delete old rendering info
        if(this->Framebuffer)
            glDeleteFramebuffers(1, &this->Framebuffer);
        if(this->Renderbuffer)
            glDeleteRenderbuffers(1, &this->Renderbuffer);
        if(this->Texture)
            glDeleteTextures(1, &this->Texture);
    }

    // For when I make multi-instancing
    glfwMakeContextCurrent(Context);

    // Board Scale
    {   
        // Scale
        glViewport(0, 0, BoardX, BoardY);

        // Generate Framebuffer
        glGenFramebuffers(1, &this->Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, this->Framebuffer); // Separate to main screen view
    }

    { // Board render configs
        OpenGLEnable(GL_DEPTH_TEST);
        OpenGLEnable(GL_CULL_FACE);
    }

    { // Texture setup for blitting
        // Create texture to render to
        glGenTextures(1, &this->Texture);
        glBindTexture(GL_TEXTURE_2D, this->Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BoardX, BoardY, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        // Create a render buffer
        glGenRenderbuffers(1, &this->Renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, this->Renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, BoardX, BoardY);
        
        // Make the render buffer load to the texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->Renderbuffer);
    }

    { // Standard Rendering Code
        // Clear Screen
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        for(Square s : BoardSquares){
            s.Render();
        }
        for(PieceSquare p : BoardPieces){
            if(&p == RenderDelay){
                break;
            }
            p.Render();
        }
        if(RenderDelay)
            RenderDelay->Render();
    }

    { // Un-bind framebuffer and copy over the board scaling it
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        OpenGLDisable(GL_DEPTH_TEST);
        OpenGLDisable(GL_CULL_FACE);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT); //If issues dissable me
        // glClear(GL_STENCIL_BUFFER_BIT);

        glViewport(0, 0, DispX,  DispY);

        glBlitFramebuffer(0, 0, BoardX, BoardY, 0, 0, DispX, DispY, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Swap display buffers (Vsync)
    glfwSwapBuffers(Context);

    //Clean
    glFlush();

    // wait for end of frame
    std::this_thread::sleep_until(next_frame);
}

void GameWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);

    // Store
    DispX = width;
    DispY = height;
} 

void GameWindow::mouse_callback(GLFWwindow* window, int button, int action, int mods){
    // If not attached, there is no point
    if(!GlobWin->IsAttached()){
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && DragStarted) {
        // Attempt to make the move
        int NewSquare = ((ceil((DispY - CursorY) / (DispY / 8.f)) - 1) * 8) + (ceil(CursorX / (DispX / 8.f)) - 1);

        // If in the move list
        std::list<Move> MoveList = GlobWin->GetHost()->GenerateMoves();

        bool Moves;

        for(Move m : MoveList){
            if (m.Start == CurrentHoveredSquare && m.End == NewSquare){
                // Make the move
                Moves = GlobWin->GetHost()->MovePiece(m);
                break;
            }
        }

        DragStarted = false;
        GlobWin->BoardPieces.at(CurrentHoveredSquare).Offset = glm::vec3(0.f);

        // Remove highlights
        for(Square* s : HighlightedSquares){
            s->ColourOffset = glm::vec4(0.f);
        }

        HighlightedSquares = {};
    }
    else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !DragStarted){
        DragStarted = true;

        // Save Coord
        PrevCursorX = CursorX;
        PrevCursorY = CursorY;

        // Find square
        CurrentHoveredSquare = ((ceil((DispY - CursorY) / (DispY / 8.f)) - 1) * 8) + (ceil(CursorX / (DispX / 8.f)) - 1);
        GlobWin->RenderDelay = &GlobWin->BoardPieces.at(CurrentHoveredSquare);

        HighlightedSquares = {};

        // Find squares that this piece can move to and highlight them red
        std::list<Move> MoveList = GlobWin->GetHost()->GenerateMoves();

        // Highlight Selected Square
        HighlightedSquares.push_back(&GlobWin->BoardSquares[CurrentHoveredSquare]);
        // GlobWin->BoardSquares[CurrentHoveredSquare].ColourOffset = glm::vec4(.25f, .25f, .25f, .25f);
        GlobWin->BoardSquares[CurrentHoveredSquare].HighlightToggle();

        for(Move m : MoveList){
            if (m.Start == CurrentHoveredSquare){
                // Highlight end
                HighlightedSquares.push_back(&GlobWin->BoardSquares[m.End]);

                GlobWin->BoardSquares[m.End].HighlightToggle();
            }
        }
    }
}

void GameWindow::mouse_cursor_callback( GLFWwindow * window, double xpos, double ypos){
    if (DragStarted){
        // Diffs
        double DiffX = xpos - CursorX;
        double DiffY = ypos - CursorY;

        double x = PrevCursorX;
        double y = PrevCursorY;

        GlobWin->BoardPieces.at(CurrentHoveredSquare).Offset = glm::vec3((xpos - x) / (DispX / 2), (y - ypos) / (DispY / 2), -.5f);
    }

    // Save new positions
    CursorX = xpos;
    CursorY = ypos;
}

void GameWindow::keyboard_processing(){
    if(!Functional){
        return; // We dont want to use a non-functional instance
    }

    if(glfwGetKey(Context, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(Context, true);
    else if(glfwGetKey(Context, GLFW_KEY_I) == GLFW_PRESS){
        InfoActive = !InfoActive;

        if(InfoActive){
            // Add extention to screen
        }
        else{
            // Remove screen extetion
        }
    }
}

void GameWindow::FrameLimit(int FPS){
    // Timer started
    next_frame += std::chrono::milliseconds(1000 / FPS);
}
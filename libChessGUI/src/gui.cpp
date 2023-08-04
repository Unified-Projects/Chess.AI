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
    GLFWwindow* window = glfwCreateWindow(SIZE_Init, SIZE_Init, "Chess", NULL, NULL);
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
    glViewport(0, 0, SIZE_Init, SIZE_Init);
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
    DispX = SIZE_Init;
    DispY = SIZE_Init;

    // Board sizing
    BoardX = SIZE_Init;
    BoardY = SIZE_Init;

    // Info Sizing
    InfoX = 0;
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
            BoardPieces.push_back(PieceSquare(x, y, Mapper->GetTex(GetType(host->board[(y*8)+x]), GetColour(host->board[(y*8)+x]))));
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
                BoardPieces.at((y * 8) + x).tex = Mapper->GetTex(GetType(host->board[(y*8)+x]), GetColour(host->board[(y*8)+x]));
            }
        }
    }

    keyboard_processing();

    // Widgets
    if(InfoActive){
        Info.Update();
    }
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

        glClearColor(66 / 255.f, 66 / 255.f, 71 / 255.f, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT); //If issues dissable me
        // glClear(GL_STENCIL_BUFFER_BIT);

        glViewport(0, 0, DispX,  DispY);

        glBlitFramebuffer(0, 0, BoardX, BoardY, 0, 0, DispX - InfoX, DispY, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    { // Restore Board render configs
        OpenGLEnable(GL_DEPTH_TEST);
        OpenGLEnable(GL_CULL_FACE);
    }

    // Post board stuff
    // Widgets
    if(InfoActive){
        Info.Render();
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

    // TODO SCALE BOARD to fill room if no Info, if Info Scale to 8x8 size and then use rest on info

    // Esure size is 8x8
    int RemainderX = width % 8;
    int RemainderY = height % 8;
    int ScaleX = width / 8;
    int ScaleY = height / 8;
    int MinScaler = std::min(ScaleX, ScaleY);

    if((ScaleX != ScaleY) && (RemainderX || RemainderY) && !GlobWin->InfoActive){
        // We need to adjust the screen
        glfwSetWindowSize(GlobWin->Context, 8 * MinScaler, 8 * MinScaler);
    }
    else if((ScaleX != ScaleY) && (RemainderX || RemainderY) && GlobWin->InfoActive){
        // We need to adjust the screen
        glfwSetWindowSize(GlobWin->Context, 8 * MinScaler + (8 * MinScaler / GlobWin->InfoWinSize), 8 * MinScaler);
        GlobWin->InfoX = (8 * MinScaler / GlobWin->InfoWinSize);
    }

    // Save for interactions
    GlobWin->BoardX = 8 * MinScaler;
    GlobWin->BoardY = 8 * MinScaler;
}

void GameWindow::mouse_callback(GLFWwindow* window, int button, int action, int mods){
    // If not attached, there is no point
    if(!GlobWin->IsAttached()){
        return;
    }

    // Ingnore from here if we are off screen
    if(CursorX > GlobWin->BoardX || CursorY > GlobWin->BoardY){
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && DragStarted) {
        // Attempt to make the move
        int NewSquare = ((ceil((GlobWin->BoardY - CursorY) / (GlobWin->BoardY / 8.f)) - 1) * 8) + (ceil(CursorX / (GlobWin->BoardX / 8.f)) - 1);

        // If in the move list
        std::list<Move> MoveList = GlobWin->GetHost()->GenerateMoves();

        bool Moves;

        for(Move m : MoveList){
            if (m.Start == CurrentHoveredSquare && m.End == NewSquare){
                if (GlobWin->OnMoveWanted){ // Is there a move callback
                    // If so see if it will let us move
                    if(GlobWin->OnMoveWanted(m, true)){ // Move permitted
                        Moves = GlobWin->GetHost()->MovePiece(m);
                    }
                    break; // Prevent making move anyway
                }

                // Otherwise make the move
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
        CurrentHoveredSquare = ((ceil((GlobWin->BoardY - CursorY) / (GlobWin->BoardY / 8.f)) - 1) * 8) + (ceil(CursorX / (GlobWin->BoardX / 8.f)) - 1);
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
                if (GlobWin->OnMoveWanted){ // Is there a move callback
                    if(!GlobWin->OnMoveWanted(m, false)){ // Move permitted
                        continue;
                    }
                }

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

        GlobWin->BoardPieces.at(CurrentHoveredSquare).Offset = glm::vec3((xpos - x) / (GlobWin->BoardX / 2), (y - ypos) / (GlobWin->BoardY / 2), -.5f);
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
    else if(glfwGetKey(Context, GLFW_KEY_I) == GLFW_PRESS && !BlockInfoPress){
        InfoActive = !InfoActive;

        if(InfoActive){
            // Remove screen extetion
            InfoX = BoardX / GlobWin->InfoWinSize;

            // Add extention to screen
            glfwSetWindowSize(Context, BoardX + InfoX, BoardY);
        }
        else{
            // Remove screen extetion
            InfoX = 0;

            glfwSetWindowSize(Context, BoardX, BoardY);
        }

        BlockInfoPress = true;
    }
    else if(glfwGetKey(Context, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(Context, GLFW_KEY_Z) == GLFW_PRESS && !UndoPressBlocker && UndoAllowed){
        host->UndoMove();
        UndoPressBlocker = true;
    }

    // Unblocking
    if(glfwGetKey(Context, GLFW_KEY_I) == GLFW_RELEASE && BlockInfoPress){
        BlockInfoPress = false;
    }
    if(glfwGetKey(Context, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(Context, GLFW_KEY_Z) == GLFW_RELEASE && UndoPressBlocker){
        UndoPressBlocker = false;
    }
}

void GameWindow::FrameLimit(int FPS){
    // Timer started
    next_frame += std::chrono::milliseconds(1000 / FPS);
}
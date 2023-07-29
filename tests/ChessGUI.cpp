// #include <libChessGUI/gui.h>
#include <libChessGUI/gui.h>
#include <libChessGUI/info.h>

#include <chess/board.h>

#include <iostream>
#include <vector>
#include <string>

#include "argparse.h"

// Board
Board board;

int main(int argc, char** argv)
{
    // Load inputs
    InputParser parser(argc, argv);
    const char* FEN = "";

    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        std::cout << "For usage of app:\n  -f [FEN]\n      Specify starting FEN" << std::endl;
        return -1;
    }

    if(parser.getCmdOption("-f").size() > 0){
        FEN = parser.getCmdOption("-f").c_str();
    }

    // Generate board
    if(!FEN[0]){
        board.InitBoard();
    }
    else{
        board.InitBoard(FEN);
    }

    // Standard for move validation and generation
    PrecomputeEdges();

    // Window setups
    GUI::GameWindow* Win = new GUI::GameWindow();

    // Widgets
    Win->Info.AddWidget(new GUI::MoveInfoWidget());

    // Attach to board
    Win->Attach(&board);

    // Main window loop
    while(!Win->IsClosing())
    {
        Win->Update();
        Win->Render();

        Win->FrameLimit(60); // 60 FPS only
    }
  
    // Kill all
    Win->Terminate();
    
    return 0;
}
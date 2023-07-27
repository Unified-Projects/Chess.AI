// #include <libChessGUI/gui.h>
#include <libChessGUI/gui.h>

#include <chess/board.h>

#include <iostream>
#include <vector>
#include <string>

// Board
Board board;

int main()
{
    // Generate board
    board.InitBoard();

    // Standard for move validation and generation
    PrecomputeEdges();

    // Window setups
    GUI::GameWindow* Win = new GUI::GameWindow();

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
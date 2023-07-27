// #include <libChessGUI/gui.h>
#include <libChessGUI/gui.h>
#include <libChessGUI/info.h>

#include <chess/board.h>

#include "argparse.h"

#include <iostream>
#include <vector>
#include <string>

// #ifdef __WIN32
// #define _WIN32_WINNT 0x0A00
// #endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

// Board
Board board;

int main()
{
    // CONFIGURATIONS
    int Port = 58008;
    bool Server = false;

    asio::error_code ec;
    // Create a "context" - essentially the platform specific interface
    asio::io_context context;
    // Get the address of somewhere we wish to connect to
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 80);
    // Create a socket, the context will deliver the implementation
    asio::ip::tcp::socket socket(context);
    // Tell socket to try and connect
    socket. connect(endpoint, ec);

    while (true){}

    // Load inputs
    InputParser parser(argc, argv);

    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        // std::cout << "For usage of test:\n  -r [Repetitions]\n     Specifies the number of times to repeat (Average calculated)\n  -l [Layer count]\n      To specify the number of layers to use\n  -f [FEN]\n      Specify starting FEN" << std::endl;
        return -1;
    }

    if(parser.getCmdOption("-p").size() > 0){
        Port = atoi(parser.getCmdOption("-l").c_str());
    }

    if(parser.getCmdOption("-s").size() > 0){
        Server = true;
    }

    // Generate board
    board.InitBoard();

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
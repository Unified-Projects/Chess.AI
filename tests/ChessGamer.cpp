#include <libChessGUI/gui.h>
#include <libChessGUI/info.h>

#include <chess/board.h>

#include <chessNet/includes.h>

#include "argparse.h"

#include <iostream>
#include <vector>
#include <memory>
#include <string>

// Connections
// Server* server = nullptr;
// Client* client = nullptr;
std::shared_ptr<Server> server;
std::shared_ptr<Client> client;

// CONFIGURATIONS
int Port = 58008;
const char* IP = "127.0.0.1";
bool ServerSettup = false;

// Board
Board board;
Colour Player;
bool MoveBypass = false;

std::array<char, 1024> Recievd;
bool MessageToHandle = false;

bool MoveCallback(Move& m){
    if(MoveBypass){
        return true;
    }

    bool Movable = board.CurrentMove == Player;

    if(Movable){ // We want to tell the other
        ChessPacket ChessData;

        ChessData.PlayedMove = std::move(m);

        // Move Extra Conversions as we cant use pointers
        if(m.Extra.type){
            ChessData.MoveExtraFrom_Square = m.Extra.From->Square;
            ChessData.MoveExtraTo_Square = m.Extra.To->Square;
        }

        // Packege and send
        if(ServerSettup){
            server->send_to_all(ChessData.to_bytes());
        }
        else{
            client->write(ChessData.to_bytes());
        }
    }

    return Movable;
}

void ServerCallback(const std::array<char, 1024>& data){
    while(MessageToHandle){
        // Idle
    }

    std::copy(data.begin(), data.end(), Recievd.begin());
    MessageToHandle = true;
}

void ClientCallback(const std::array<char, 1024>& data){
    while(MessageToHandle){
        // Idle
    }

    std::copy(data.begin(), data.end(), Recievd.begin());
    MessageToHandle = true;
}

void ServerMode(){
    // server = new Server(1234, ServerCallback);
    server = std::make_shared<Server>(Port, ServerCallback);

    // Wait for a connections
    server->wait_for_new_connection();

    std::cout << "Connected Client, proceeding to send chess initialiser!" << std::endl;

    // Now tell client their colour
    server->send_to_all(ChessSetupPacket(BLACK, "").to_bytes());

    // Setup Player
    Player = WHITE;

    // Generate board
    board.InitBoard();

    // Standard for move validation and generation
    PrecomputeEdges();

    // Window setups
    GUI::GameWindow* Win = new GUI::GameWindow();

    // Add callbacks
    Win->OnMoveWanted = MoveCallback;

    // Widgets
    Win->Info.AddWidget(new GUI::MoveInfoWidget());

    // Attach to board
    Win->Attach(&board);

    // Main window loop
    while(!Win->IsClosing())
    {
        Win->Update();

        // Need to handle move from other
        if(MessageToHandle){
            // Load to a move packet
            ChessPacket ChessData;
            ChessData.load_from_bytes(Recievd);

            // Correct Move Packet
            Move m = ChessData.PlayedMove;
            m.Extra.From = board.board[ChessData.MoveExtraFrom_Square];
            m.Extra.To = board.board[ChessData.MoveExtraTo_Square];

            std::cout << "From: " << m.Start << " To: " << m.End << std::endl; 

            // Play the move
            MoveBypass = true;
            board.MovePiece(m);
            MoveBypass = false;

            MessageToHandle = false;
        }

        Win->Render();
        Win->FrameLimit(60); // 60 FPS only
    }

    // Kill all
    Win->Terminate();

    // Close the server
    server->close();
}

void ClientMode(){
    // client = new Client("127.0.0.1", "1234", ClientCallback);
    client = std::make_shared<Client>(IP, std::to_string(Port), ClientCallback);

    // Wait for initialiser
    ChessSetupPacket InitData;
    client->wait_for_message();
    InitData.load_from_bytes(Recievd);
    MessageToHandle = false;

    // Player
    Player = InitData.col;

    // Generate board
    if(InitData.FEN[0])
        board.InitBoard(InitData.FEN);
    else
        board.InitBoard();

    // Standard for move validation and generation
    PrecomputeEdges();

    // Window setups
    GUI::GameWindow* Win = new GUI::GameWindow();

    // Add callbacks
    Win->OnMoveWanted = MoveCallback;

    // Widgets
    Win->Info.AddWidget(new GUI::MoveInfoWidget());

    // Attach to board
    Win->Attach(&board);

    // Main window loop
    while(!Win->IsClosing())
    {
        Win->Update();

        // Need to handle move from other
        if(MessageToHandle){
            // Load to a move packet
            ChessPacket ChessData;
            ChessData.load_from_bytes(Recievd);

            // Correct Move Packet
            Move m = ChessData.PlayedMove;
            m.Extra.From = board.board[ChessData.MoveExtraFrom_Square];
            m.Extra.To = board.board[ChessData.MoveExtraTo_Square];

            std::cout << "From: " << m.Start << " To: " << m.End << std::endl; 

            // Play the move
            MoveBypass = true;
            board.MovePiece(m);
            MoveBypass = false;

            MessageToHandle = false;
        }

        Win->Render();
        Win->FrameLimit(60); // 60 FPS only
    }

    // Kill all
    Win->Terminate();

    // Close the client
    client->close();
}

int main(int argc, char** argv)
{
    // Load inputs
    InputParser parser(argc, argv);

    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        // std::cout << "For usage of test:\n  -r [Repetitions]\n     Specifies the number of times to repeat (Average calculated)\n  -l [Layer count]\n      To specify the number of layers to use\n  -f [FEN]\n      Specify starting FEN" << std::endl;
        return -1;
    }

    if(parser.getCmdOption("-p").size() > 0){
        Port = atoi(parser.getCmdOption("-p").c_str());
    }

    if(parser.getCmdOption("-a").size() > 0){
        IP = parser.getCmdOption("-a").c_str();
    }

    if(parser.cmdOptionExists("-s")){
        ServerSettup = true;
        ServerMode();
    }
    else{
        ClientMode();
    }

    return 0;
}
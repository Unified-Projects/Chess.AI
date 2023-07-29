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
const char* FEN = "";
bool ServerSettup = false;

// Board
Board board;
Colour Player = WHITE;
bool MoveBypass = false;

std::array<char, 1024> Recievd;
bool MessageToHandle = false;

bool MoveCallback(Move& m, bool played){
    if(MoveBypass){
        return true;
    }

    bool Movable = board.CurrentMove == Player;

    if(!played){
        return Movable;
    }

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

void MainGame(const char * FEN){
    // Generate board
    if(FEN[0])
        board.InitBoard(FEN);
    else
        board.InitBoard();

    // Standard for move validation and generation
    PrecomputeEdges();

    // Window setups
    GUI::GameWindow* Win = new GUI::GameWindow();

    // Dissable Undoing
    Win->UndoAllowed = false;

    // Add callbacks
    Win->OnMoveWanted = MoveCallback;

    // Widgets
    Win->Info.AddWidget(new GUI::MoveInfoWidget());
    Win->Info.AddWidget(new GUI::BlankWidget());
    Win->Info.AddWidget(new GUI::PieceWidget(KING, Player));

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
            if(ChessData.MoveExtraFrom_Square >= 0 && ChessData.MoveExtraFrom_Square <= 63){
                m.Extra.From = board.board[ChessData.MoveExtraFrom_Square];
            }
            else{
                m.Extra.From = new Piece();
            }
            if(ChessData.MoveExtraTo_Square >= 0 && ChessData.MoveExtraTo_Square <= 63){
                m.Extra.To = board.board[ChessData.MoveExtraTo_Square];
            }
            else{
                m.Extra.To = new Piece();
            }

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
}

void ServerMode(){
    // server = new Server(1234, ServerCallback);
    server = std::make_shared<Server>(Port, ServerCallback);

    // Wait for a connections
    server->wait_for_new_connection();

    std::cout << "Connected Client, proceeding to send chess initialiser!" << std::endl;

    // Now tell client their colour
    server->send_to_all(ChessSetupPacket((Player == WHITE) ? BLACK : WHITE, FEN).to_bytes());

    // Actual GUI Stuff
    MainGame(FEN);

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

    // Actual GUI Stuff
    MainGame(InitData.FEN);

    // Close the client
    client->close();
}

int main(int argc, char** argv)
{
    // Load inputs
    InputParser parser(argc, argv);

    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        std::cout << "For usage of app:\n  -p [Port]\n     Specifies the connection port\n  -a [IP Address]\n      To specify the server IP\n  -f [FEN]\n      Specify starting FEN\n  -s\n      Run as server\n  -b\n      Server setting to not be White by default" << std::endl;
        return -1;
    }

    if(parser.getCmdOption("-p").size() > 0){
        Port = atoi(parser.getCmdOption("-p").c_str());
    }

    if(parser.getCmdOption("-a").size() > 0){
        IP = parser.getCmdOption("-a").c_str();
    }

    if(parser.getCmdOption("-f").size() > 0){
        FEN = parser.getCmdOption("-f").c_str();
    }

    if(parser.cmdOptionExists("-s")){
        if (parser.cmdOptionExists("-b")){
            Player = BLACK;
        }

        ServerSettup = true;
        ServerMode();
    }
    else{
        ClientMode();
    }

    return 0;
}
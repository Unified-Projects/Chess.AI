#include <chess/board.h>
#include <chess/piece.h>

#include "argparse.h"
#include "logger.h"

#include <thread>
#include <iostream>
#include <time.h>
#include <stack>
#include <list>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <cstring>

void TestPossibleMoves(Board* board, int layer, int& moves, int& checkmates, uint64_t& iterations){
    if(layer == 0){
        return;
    }

    std::list<Move> Moves = board->GenerateMoves();

    for (Move m : Moves){
        // std::cout << m.Start << " " << m.End << std::endl;

        bool ValidMove = board->MovePiece(m);

        TestPossibleMoves(board, layer-1, moves, checkmates, iterations);

        // Increments
        iterations++;

        if(ValidMove && layer == 1){
            moves++;
            // board->LogBoard();
        }

        // If valid move undo
        if(ValidMove){
            board->UndoMove();
        }
    }
}

struct CustomBoard{
    const char* FEN;
    int MaxLayers = 0;
    uint64_t LayerNodes[10];
    uint64_t Checkmates[10];
    bool CheckmatesToggle = true;
};

std::vector<CustomBoard> Boards = {
    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", 6, {48, 2039, 97862, 4085603, 193690690, 8031647685}, {0, 0, 1, 43, 30171, 360003}},
    {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, {6, 264, 9467, 422333, 15833292, 706045033}, {0, 0, 22, 5, 50562, 81076}},
    {"r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ", 6, {6, 264, 9467, 422333, 15833292, 706045033}, {0, 0, 22, 5, 50562, 81076}},
    {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ", 5, {44, 1486, 62379, 2103487, 89941194}, {}, false}
};

int main(int argc, char** argv){
    // Load inputs
    InputParser parser(argc, argv);

    // Standard
    PrecomputeEdges();

    // Set defaults
    int Repetitions = 1;

    // Get Layer count from parser
    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        std::cout << "For usage of test:\n  -r [Repetitions]\n     Specifies the number of times to repeat (Average calculated)" << std::endl;
        return -1;
    }
    
    // Get thread option
    if(parser.getCmdOption("-r").size() > 0){
        Repetitions = atoi(parser.getCmdOption("-r").c_str());
    }

    for (CustomBoard b : Boards){
        // New Board
        Board board;
        board.InitBoard(b.FEN);

        for (int Layer = 1; Layer <= b.MaxLayers; Layer++){
            // Averages
            int Moves = 0;
            int Checkmates = 0;
            uint64_t Iterations = 0;
            double Times = 0;

            for (int i = 1; i <= Repetitions; i++){
                clock_t StartTime = clock();

                // Actual Layer Test
                TestPossibleMoves(&board, Layer, Moves, Checkmates, Iterations);

                clock_t EndTime = clock();
                double TimeTaken = (double) (EndTime-StartTime) / CLOCKS_PER_SEC * 1000.0;
                Times += TimeTaken;
            }

            // Actuall Testing
            if(Moves/Repetitions != b.LayerNodes[Layer-1]){
                // Wrong Ammount of moves so failed
                std::cout << FMT("Error: ", ERR) << FMT("Wrong Move Count. Got: ", NORMAL) << Moves / Repetitions << FMT(" Where Expecting: ", NORMAL) << b.LayerNodes[Layer-1] << std::endl;
                std::cout << FMT("Info: ", WARN) << Moves / Repetitions << FMT(" Moves and ", NORMAL) << Checkmates / Repetitions << FMT(" Checkmates ", NORMAL) << FMT("(" + std::to_string(Times/Repetitions) + "ms)", TIME) << std::endl;
                return -1;
            }

            if(b.CheckmatesToggle){
                if(b.Checkmates[Layer-1] != Checkmates / Repetitions){
                    // Wrong ammount of checkmates
                    std::cout << FMT("Error: ", ERR) << FMT("Wrong Checkmate Count. Got: ", NORMAL) << Checkmates / Repetitions << FMT(" Where Expecting: ", NORMAL) << b.Checkmates[Layer-1] << std::endl;
                    std::cout << FMT("Info: ", WARN) << Moves / Repetitions << FMT(" Moves and ", NORMAL) << Checkmates / Repetitions << FMT(" Checkmates ", NORMAL) << FMT("(" + std::to_string(Times/Repetitions) + "ms)", TIME) << std::endl;
                    return -1;
                }
            }

            std::cout << FMT("Success: ", SUCCESS) << FMT("Correct Counts. Got: ", NORMAL) << Moves / Repetitions << FMT(" Moves and ", NORMAL) << Checkmates / Repetitions << FMT(" Checkmates ", NORMAL) << FMT("(" + std::to_string(Times/Repetitions) + "ms)", TIME) << std::endl;
        }
    }
    return 0;
}
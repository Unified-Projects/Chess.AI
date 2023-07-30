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

        
        if(ValidMove && board->UpdateMate()){
            // Checkmate
            checkmates++;
        }
        else
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

const uint64_t ShannonLimit[] = {
    20,
    400,
    8902,
    197281,
    4865609,
    119060324,
    3195901860,
    84998978956,
    2439530234167,
    69352859712417,
    2097651003696806,
    62854969236701747,
    1981066775000396239
};
const uint64_t ShannonCheckmate[] = {
    0,
    0,
    0,
    8,
    347,
    10828,
    435767,
    9852036,
    400191963,
    8790619155,
    362290010907,
    8361091858959,
    346742245764219
};

const bool ShannonCheckmateToggle[] = {
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    false,
    false,
    false
};

int main(int argc, char** argv){
    // Load inputs
    InputParser parser(argc, argv);

    // Standard
    PrecomputeEdges();

    // Set defaults
    int Repetitions = 1;
    int LayerCount = 5;
    bool UsingCustomFen = false;
    const char* CustomStartingFen = "";

    // Get Layer count from parser
    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        std::cout << "For usage of test:\n  -r [Repetitions]\n     Specifies the number of times to repeat (Average calculated)\n  -l [Layer count]\n      To specify the number of layers to use\n  -f [FEN]\n      Specify starting FEN" << std::endl;
        return -1;
    }

    // Get Layer count from parser
    if(parser.getCmdOption("-l").size() > 0){
        LayerCount = atoi(parser.getCmdOption("-l").c_str());
    }
    
    // Get thread option
    if(parser.getCmdOption("-r").size() > 0){
        Repetitions = atoi(parser.getCmdOption("-r").c_str());
    }

    // Get input option
    if(parser.getCmdOption("-f").size() > 0){
        CustomStartingFen = parser.getCmdOption("-f").c_str();
        UsingCustomFen = true;
    }

    // One board needed
    Board board;
    if(!UsingCustomFen)
        board.InitBoard();
    else
        board.InitBoard(CustomStartingFen); // TODO: Future issue (This wont load whole FEN string only the board due to spaces in the token)

    // Nodes per second calculations
    float NPS = 0;
    clock_t TimeTaken = 0;

    // Failed
    bool Failed = false;

    for (int Layer = 1; Layer <= LayerCount; Layer++){
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

        // Save for NPS Calculations
        NPS += Moves;
        TimeTaken += Times;

        // Actuall Testing
        if(Moves/Repetitions != ShannonLimit[Layer-1] && !UsingCustomFen){
            // Wrong Ammount of moves so failed
            std::cout << FMT("Error: ", ERR) << FMT("Wrong Move Count. Got: ", NORMAL) << Moves / Repetitions << FMT(" Where Expecting: ", NORMAL) << ShannonLimit[Layer-1] << std::endl;
            std::cout << FMT("Info: ", WARN) << Moves / Repetitions << FMT(" Moves and ", NORMAL) << Checkmates / Repetitions << FMT(" Checkmates ", NORMAL) << FMT("(" + std::to_string(Times/Repetitions) + "ms)", TIME) << std::endl;
            Failed = true;
            break;
        }

        if(ShannonCheckmateToggle[Layer-1] && !UsingCustomFen){
            if(ShannonCheckmate[Layer-1] != Checkmates / Repetitions){
                // Wrong ammount of checkmates
                std::cout << FMT("Error: ", ERR) << FMT("Wrong Checkmate Count. Got: ", NORMAL) << Checkmates / Repetitions << FMT(" Where Expecting: ", NORMAL) << ShannonCheckmate[Layer-1] << std::endl;
                std::cout << FMT("Info: ", WARN) << Moves / Repetitions << FMT(" Moves and ", NORMAL) << Checkmates / Repetitions << FMT(" Checkmates ", NORMAL) << FMT("(" + std::to_string(Times/Repetitions) + "ms)", TIME) << std::endl;
                Failed = true;
                break;
            }
        }
        
        std::cout << FMT("Success: ", SUCCESS) << FMT("Correct Counts. Got: ", NORMAL) << Checkmates / Repetitions << FMT(" Checkmates and ", NORMAL) << Moves / Repetitions << FMT(" Moves ", NORMAL) << FMT("(" + std::to_string(Times/Repetitions) + "ms)", TIME) << std::endl;
    }

    // NPS Logging (Reference: Qperft has 190 Million NPS)
    std::cout << FMT("NPS: ", WARN) << (NPS / (TimeTaken / 1000)) << std::endl;

    // End (Failed needed for CTests)
    return (Failed) ? -1 : 0;
}
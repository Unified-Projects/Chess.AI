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

std::list<std::string> ValidFens = {};
std::list<std::string> FoundFens = {};

int Validated = 0;
int Invalid = 0;

const char* InputFile = "./Valid.txt";
const char* OutputFile = "./Issues.txt";

int LoadFens(){
    std::ifstream inputFile(InputFile);  // Replace "file.txt" with the actual file path

    if (!inputFile.is_open()) {
        std::cout << FMT("Error: ", ERR) << FMT("Unable to load file.", NORMAL) << std::endl;
        return -1;
    }

    std::string line;

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string item;

        while (std::getline(ss, item, ',')) {
            ValidFens.push_back(item);
        }
    }

    inputFile.close();

    return 0;
}

void StoreBoard(Board* b){
    std::ofstream fileOUT(OutputFile, std::ios::app); // open filename.txt in append mode

    // Convert Board Moves to String
    std::string Moves = "";

    for (MoveCache m : b->PlayedMoves){
        Moves += Board::typeMapper[m.MovedPiece->GetT()];
        Moves += "." + std::string((m.MovedPiece->GetC() == WHITE) ? "W" : "B");
        Moves += ":";
        Moves += std::to_string(m.move.Start);
        Moves += ":";
        Moves += std::to_string(m.move.End);
        Moves += "\n";
    }

    fileOUT << b->ConvertToFen() << "\n" << Moves << std::endl; // append "some stuff" to the end of the file

    fileOUT.close(); // close the file
}

void StoreText(std::string Text){
    std::ofstream fileOUT(OutputFile, std::ios::app); // open filename.txt in append mode

    // Convert Board Moves to String
    Text = Text;

    fileOUT << Text << std::endl; // append "some stuff" to the end of the file

    fileOUT.close(); // close the file
}

// Caching for found valid fens
void CacheFoundFen(std::string FEN){
    for (std::string s : FoundFens){
        // Check if already found
        if(FEN.find(s) != std::string::npos){
            return; // Found and so already stored
        }
    }

    // Cache
    FoundFens.push_back(FEN);
}

// Skipping Point
uint64_t SkipTo = 0;

bool ValidateFen(Board* b){
    if (ValidFens.empty()) {
        LoadFens();
        std::cout << "Loaded Fens" << std::endl;
    }

    if(SkipTo != Validated){
        Validated++;
        return true; // Skipped
    }

    // Convert board to fen
    std::string boardFen = b->ConvertToFen();

    // Get str length
    int i = boardFen.size();

    for(std::string fen : ValidFens){
        if(fen.find(boardFen) != std::string::npos){
            Validated++;

            // DEBUG PURPOSES ONLY
            if(!(Validated % 1000)){
                std::cout << FMT("Validated: ", NORMAL) << Validated << std::endl;
            }

            // Cache it
            CacheFoundFen(fen);

            return true;
        }
    }

    // Fen is invalid, cache it!
    StoreBoard(b);

    Validated++;
    Invalid++;

    // DEBUG PURPOSES ONLY
    if(!(Validated % 1000)){
        std::cout << FMT("Validated: ", NORMAL) << Validated << std::endl;
    }

    return false;
}

void ThreadedPossibleMoves(Board* board, int splitcoutIndex, int splitcout, int layer){
    if(layer == 0){
        return;
    }

    std::list<Move> Moves = board->GenerateMoves();

    for (Move m : Moves){

        if(layer == 1 && splitcoutIndex != splitcout - 1){
            splitcoutIndex++;
            continue;
        }
        else if(layer == 1 && splitcoutIndex == splitcout - 1){
            splitcoutIndex = 0;
        }

        bool ValidMove = board->MovePiece(m);
        
        if(ValidMove && board->UpdateMate()){
            // Checkmate
        }
        else
            ThreadedPossibleMoves(board, splitcoutIndex, splitcout, layer-1);

        // If layer == 1:
        if(layer == 1 && ValidMove){
            ValidateFen(board);
        }

        // If valid move undo
        if(ValidMove){
            board->UndoMove();
        }
    }

    return;
}

int ThreadedValidations(int threadcount, int layerNumber, const char * FEN){
    std::list<std::thread*> ActiveThreads;

    // Load fens or issues arise
    if(LoadFens()){
        return -1;
    }

    for(int i = 0; i < threadcount; i++){
        Board* board = new Board(); // Each need their own board
        if(!FEN[0]){
            board->InitBoard();
        }
        else{
            board->InitBoard(FEN);
        }
        std::thread* t = new std::thread(ThreadedPossibleMoves, board, i, threadcount, layerNumber);

        ActiveThreads.push_back(t);

        std::cout << FMT("Info: ", WARN) << FMT("Thread " + std::to_string(i) + " Started", NORMAL) << std::endl;
    }

    // Finish threads
    while (!ActiveThreads.empty()){
        std::thread* t = ActiveThreads.back();

        t->join(); // Wait till ended
        ActiveThreads.remove(t); // Remove from queue
        // Delete thread?
    }

    // Validated log
    std::cout << FMT("Info: ", WARN) << FMT("Validated: ", NORMAL) << Validated << FMT(" Fens and found ", NORMAL) << Invalid << FMT(" Invalid Fens", NORMAL) << std::endl;

    // Did we miss any?
    std::cout << FMT("Info: ", WARN) << FMT("Found: ", NORMAL) << FoundFens.size() << FMT(" Needed: ", NORMAL) << ValidFens.size() << std::endl;

    for (std::string s : ValidFens){
        bool Found = false;
        for (std::string s2 : FoundFens){
            // Check if already found
            if(s.find(s2) != std::string::npos){
                Found = true;
            }
        }

        if(!Found){
            StoreText("MISSED: " + s);
        }
    }

    if(Invalid || FoundFens.size() != ValidFens.size()){
        return -1;
    }

    return 0;
}

int main(int argc, char** argv){
    // Load inputs
    InputParser parser(argc, argv);

    // Standard
    PrecomputeEdges();

    // Set defaults
    int ThreadCount = 4;
    int LayerCount = 5;
    const char* FEN = "";

    // Get Layer count from parser
    if(parser.cmdOptionExists("-h") || parser.cmdOptionExists("--help")){
        std::cout << "For usage of test:\n  -f [FEN]\n     Specifies what FEN it's using\n  -t [Thread count]\n     Specifies the number of threads to use\n  -l [Layer count]\n     To specify the number of layers to use\n  -i [Input file path]\n     To specify the file that the valid fens are csv in\n  -o [Output file path]\n     To specify where all invalid results are to be outputted to\  -s [Offset]\n     Set a starting point for validations so that you don't have to repeat" << std::endl;
        return -1;
    }

    // Get Layer count from parser
    if(parser.getCmdOption("-l").size() > 0){
        LayerCount = atoi(parser.getCmdOption("-l").c_str());
    }
    
    // Get thread option
    if(parser.getCmdOption("-t").size() > 0){
        ThreadCount = atoi(parser.getCmdOption("-t").c_str());
    }

    // Get skipto option
    if(parser.getCmdOption("-s").size() > 0){
        SkipTo = atoi(parser.getCmdOption("-s").c_str());
    }

    // Get input option
    if(parser.getCmdOption("-i").size() > 0){
        InputFile = parser.getCmdOption("-i").c_str();
    }

    // Get output option
    if(parser.getCmdOption("-o").size() > 0){
        OutputFile = parser.getCmdOption("-o").c_str();
    }

    // Get fen option
    if(parser.getCmdOption("-f").size() > 0){
        FEN = parser.getCmdOption("-f").c_str();
    }

    // Run validator
    return ThreadedValidations(ThreadCount, LayerCount, FEN);
}
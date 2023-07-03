#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>
#include <time.h>
#include <stack>
#include <list>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <cstring>

// TODO: REALISE THAT THE FEN COULD HAVE ROLES REVERSED SO BLACK START ON BOTTOM NOT TOP (HANDLING FOR THIS!)

// Sacrafice to the chess gods
/*
                      🕯️
              🕯️             🕯️

        🕯️                          🕯️


    🕯️               🗡️🕴️                🕯️


        🕯️                          🕯️

              🕯️             🕯️

                      🕯️
*/

/*

            NEW IDEA

    So we are going to want to
    recode and remove all the valid
    move functions. We want to instead
    calculate all the possible moves
    for that piece and iterate over
    them individally. So instead of
    IsValidMove we have GenerateMoves.
    This will have a more efficient
    implementation of everything.
    Since we know this move is valid
    we can always move piece with the
    valid move structure. Otherwise,
    using a read/write structure
    invalid move we can allow the player
    to attempt a move to be checked against
    the valid moves.

    I know a re-write is annoying.
    But I think this will be a massive
    improvement.

*/

std::list<std::string> ValidFens = {};

void LoadFens(const char* FilePath){
    std::ifstream inputFile(FilePath);  // Replace "file.txt" with the actual file path

    if (!inputFile.is_open()) {
        std::cout << "Oppsie You FUCKED Up" << std::endl;
        return;
    }

    std::string line;

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string item;

        while (std::getline(ss, item, ',')) {
            ValidFens.push_back(item);
            // std::cout << item << std::endl;  // Log each item
        }
    }

    inputFile.close();

    // DEBUG: DEBUG PERPOSES ONLY
    std::ofstream outputFile("../FENGen/Load.txt");  // Replace "file.txt" with the actual file path

    if (!outputFile.is_open()) {
        std::cout << "Oppsie You FUCKED Up" << std::endl;
        return;
    }

    // Recombine the items and write to the file
    for (const std::string& item : ValidFens) {
        outputFile << item << '\n';
    }

    outputFile.close();

    return;
}

void StoreBoard(Board* b, const char* Filepath = "Fens.txt"){
    std::ofstream fileOUT(Filepath, std::ios::app); // open filename.txt in append mode

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

void StoreText(std::string Text, const char* Filepath = "Fens.txt"){
    std::ofstream fileOUT(Filepath, std::ios::app); // open filename.txt in append mode

    // Convert Board Moves to String
    Text = Text + "\n";

    fileOUT << Text << std::endl; // append "some stuff" to the end of the file

    fileOUT.close(); // close the file
}

// Caching for found valid fens
std::list<std::string> FoundFens = {};
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

// Temporary debug
int Validated = 0;
bool ValidateFen(Board* b){

    if (ValidFens.empty()) {
        LoadFens("../FENGen/fens.txt");
        std::cout << "Loaded Fens" << std::endl;
    }

    // Convert board to fen
    std::string boardFen = b->ConvertToFen();

    // Get str length
    int i = boardFen.size();

    for(std::string fen : ValidFens){
        if(fen.find(boardFen) != std::string::npos){
            Validated++;

            // DEBUG PURPOSES ONLY
            if(!(Validated % 100)){
                std::cout << "Validated: " << Validated << std::endl;
            }

            // Cache it
            CacheFoundFen(fen);

            return true;
        }
    }

    // Fen is invalid, cache it!
    StoreBoard(b);

    Validated++;

    // DEBUG PURPOSES ONLY
    if(!(Validated % 100)){
        std::cout << "Validated: " << Validated << std::endl;
    }

    return false;
}

// uint64_t Iterations = 0;
// uint64_t Moves = 0;
// uint64_t Checkmates = 0;

// int RecursedPossibleMoves(Board* b, int LayerNumber = 1, Colour c = WHITE){
//     if(LayerNumber == 0){
//         Moves++;
//         return 0;
//     }

//     std::list<Piece*> currentLayer = (c == WHITE) ? b->GetWhitePieces() : b->GetBlackPieces();

//     for (Piece* p : currentLayer){
//         for (std::pair<int, int> move : b->MoveGen(p->GetT())){
//             // Play the move
//             bool movedPiece = b->MovePiece(p->X, p->Y, p->X + move.first, p->Y + move.second);

//             if(movedPiece && !b->UpdateCheckmate()){
//                 RecursedPossibleMoves(b, LayerNumber-1, (c == 0xFF) ? BLACK : WHITE);
//             }
//             else if (b->UpdateCheckmate()){
//                 Moves++;
//                 Checkmates++;
//             }

//             Iterations++;

//             if(movedPiece){
//                 b->UndoMove();
//             }
//         }
//     }

//     return 0;
// }

// VALIDATIONS
#include <thread>
void ThreadedPossibleMoves(Board* board, int splitcoutIndex, int splitcout, int layer){
    if(layer == 0){
        return;
    }

    std::list<Move> Moves = board->GenerateMoves();

    for (Move m : Moves){
        if(layer == 1 && splitcoutIndex != splitcout){
            splitcoutIndex++;
            continue;
        }
        else if(layer == 1 && splitcoutIndex == splitcout){
            splitcoutIndex = 0;
        }

        bool ValidMove = board->MovePiece(m);

        // Validator Code
        if(layer == 1 && ValidMove){
            // Attempt move validation
            ValidateFen(board);
        }

        ThreadedPossibleMoves(board, splitcoutIndex, splitcout, layer-1);

        // If valid move undo
        if(ValidMove){
            // board->LogBoard();
            board->UndoMove();
        }
    }
}

void ThreadedValidations(int threadcount, int layerNumber){
    std::list<std::thread*> ActiveThreads;

    // Load fens or issues arise
    LoadFens("../FENGen/fens.txt");

    for(int i = 0; i < threadcount; i++){
        Board* board = new Board(); // Each need their own board
        board->InitBoard();
        std::thread* t = new std::thread(ThreadedPossibleMoves, board, i, threadcount, layerNumber);

        ActiveThreads.push_back(t);

        std::cout << "Thread" << i << ": Started" << std::endl;
    }

    // Finish threads
    while (!ActiveThreads.empty()){
        std::thread* t = ActiveThreads.back();

        t->join(); // Wait till ended
        ActiveThreads.remove(t); // Remove from queue
        // Delete thread?
    }

    // Did we miss any?
    std::cout << "Found: " << FoundFens.size() << " Needed: " << ValidFens.size() << std:: endl;

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
}
// VALIDATIONS

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
        }

        // If valid move undo
        if(ValidMove){
            // board->LogBoard();
            board->UndoMove();
        }
    }
}

int main() {
    PrecomputeEdges();

    Board board;
    board.InitBoard();

    board.LogBoard();

    bool Validator = true;
    bool Comparitor = true;

    if(Validator && Comparitor) { // Multi-threaded validations
        ThreadedValidations(2, 2);
    }

    // Testing
    if(Validator && !Comparitor) {
        int Repeats = 1; // Allow average calculations
        int MaxLayers = 2; // Change depth of test

        for (int Layer = 1; Layer <= MaxLayers; Layer++){
            // Averages
            int Moves = 0;
            int Checkmates = 0;
            uint64_t Iterations = 0;
            double Times = 0;

            for (int i = 1; i <= Repeats; i++){
                clock_t StartTime = clock();

                // Actual Layer Test
                TestPossibleMoves(&board, Layer, Moves, Checkmates, Iterations);

                clock_t EndTime = clock();
                double TimeTaken = (double) (EndTime-StartTime) / CLOCKS_PER_SEC * 1000.0;
                Times += TimeTaken;
            }

            std::cout << "All Move Calculations took: " << Times / Repeats << "ms for " << Layer << " Layers and " << Moves / Repeats << " Moves and " << Checkmates / Repeats << " Checkmates" << std::endl;
                std::cout << "      And a average of " << (Times / Repeats) / (Iterations / Repeats / 1000) << "ms per 1000 Iterations" << std::endl;
        }
    }

    // Validations
    // if(true){
    //     std::cout << "Beggining threaded Tests:" << std::endl;
    //     OptimisedValidationTests(24, 5);

    //     while (true){
    //         // Idle
    //     }
    // }

    // if(true){// Efficency testings
    //     int Repetitions = 1;
    //     int MaxLayers = 4;

    //     std::cout << "Recursed Timing Tests:" << std::endl;

    //     {
    //         for ( int Layers = 1; Layers <= MaxLayers; Layers++){
    //             double Sum = 0;

    //             for(int i = 1; i <= Repetitions; i++){
    //                 clock_t start = clock();
    //                 RecursedPossibleMoves(&board, Layers);
    //                 clock_t end = clock();
    //                 double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
    //                 Sum += time;
    //             }
    //             double time = Sum / Repetitions;

    //             std::cout << "All Move Calculations took: " << time << "ms for " << Layers << " Layers and " << Moves/Repetitions << " Moves and " << Checkmates / Repetitions << " Checkmates" << std::endl;
    //             std::cout << "      And a average of " << time/(Iterations/Repetitions/1000) << "ms per 1000 Iterations" << std::endl;
    //             Iterations = 0;
    //             Moves = 0;
    //             Checkmates = 0;
    //         }
    //     }
    // }

    return 0;
}

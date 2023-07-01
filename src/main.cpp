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

// std::list<std::string> ValidFens = {};

// void LoadFens(const char* FilePath){
//     std::ifstream inputFile(FilePath);  // Replace "file.txt" with the actual file path

//     if (!inputFile.is_open()) {
//         std::cout << "Oppsie You FUCKED Up" << std::endl;
//         return;
//     }

//     std::string line;

//     while (std::getline(inputFile, line)) {
//         std::stringstream ss(line);
//         std::string item;

//         while (std::getline(ss, item, ',')) {
//             ValidFens.push_back(item);
//             // std::cout << item << std::endl;  // Log each item
//         }
//     }

//     inputFile.close();

//     // DEBUG: DEBUG PERPOSES ONLY
//     std::ofstream outputFile("../FENGen/Load.txt");  // Replace "file.txt" with the actual file path

//     if (!outputFile.is_open()) {
//         std::cout << "Oppsie You FUCKED Up" << std::endl;
//         return;
//     }

//     // Recombine the items and write to the file
//     for (const std::string& item : ValidFens) {
//         outputFile << item << '\n';
//     }

//     outputFile.close();

//     return;
// }

// void StoreBoard(Board* b, const char* Filepath = "Fens.txt"){
//     std::ofstream fileOUT(Filepath, std::ios::app); // open filename.txt in append mode

//     // Convert Board Moves to String
//     std::string Moves = "";

//     for (MoveCache m : b->PlayedMoves){
//         Moves += Board::typeMapper[m.MovedPiece->GetT()];
//         Moves += "." + std::string((m.MovedPiece->GetC() == WHITE) ? "W" : "B");
//         Moves += ":";
//         Moves += std::to_string(m.StartX) + ", " + std::to_string(m.StartY);
//         Moves += ":";
//         Moves += std::to_string(m.EndX) + ", " + std::to_string(m.EndY);
//         Moves += "\n";
//     }

//     fileOUT << b->ConvertToFen() << "\n" << Moves << std::endl; // append "some stuff" to the end of the file

//     fileOUT.close(); // close the file
// }

// // Temporary debug
// int Validated = 0;
// bool ValidateFen(Board* b){

//     // Restore previous positions
//     if(Validated < 3127900){
//         Validated++;
//         return true;
//     }

//     if (ValidFens.empty()) {
//         LoadFens("../FENGen/fens.txt");
//         std::cout << "Loaded Fens" << std::endl;
//     }

//     // Convert board to fen
//     std::string boardFen = b->ConvertToFen();

//     // Get str length
//     int i = boardFen.size();

//     for(std::string fen : ValidFens){
//         if(fen.find(boardFen) != std::string::npos){
//             Validated++;

//             // DEBUG PURPOSES ONLY
//             if(!(Validated % 100)){
//                 std::cout << "Validated: " << Validated << std::endl;
//             }

//             return true;
//         }
//     }

//     // Fen is invalid, cache it!
//     StoreBoard(b);

//     Validated++;
    
//     // DEBUG PURPOSES ONLY
//     // if(!(Validated % 100)){
//         std::cout << "Validated: " << Validated << std::endl;
//     // }

//     return false;
// }

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

// // VALIDATIONS
// #include <thread>
// int SplitRecursedPossibleMoves(Board* b, int splitcoutIndex, int splitcout, int LayerNumber = 1, Colour c = WHITE){
//     if(LayerNumber == 0){
//         Moves++;
//         return 0;
//     }

//     std::list<Piece*> currentLayer = (c == WHITE) ? b->GetWhitePieces() : b->GetBlackPieces();

//     for (Piece* p : currentLayer){
//         for (std::pair<int, int> move : b->MoveGen(p->GetT())){
//             if(LayerNumber == 1 && splitcoutIndex != splitcout){
//                 splitcoutIndex++;
//                 continue;
//             }
//             else if(LayerNumber == 1 && splitcoutIndex == splitcout){
//                 splitcoutIndex = 0;
//             }

//             bool movedPiece = b->MovePiece(p->X, p->Y, p->X + move.first, p->Y + move.second);

//             // Validator Code
//             if(LayerNumber == 1 && movedPiece){
//                 // Attempt move validation
//                 ValidateFen(b);
//             }

//             if(movedPiece && !b->UpdateCheckmate()){
//                 SplitRecursedPossibleMoves(b, splitcoutIndex, splitcout, LayerNumber-1, (c == 0xFF) ? BLACK : WHITE);
//             }

//             // Iterations++;
//             if(LayerNumber == 1)
//                 splitcoutIndex++;

//             if(movedPiece){
//                 b->UndoMove();
//             }
//         }
//     }

//     return 0;
// }

// void OptimisedValidationTests(int threadcount, int layerNumber){
//     std::list<std::thread*> ActiveThreads;

//     // Load fens or issues arise
//     LoadFens("../FENGen/fens.txt");

//     for(int i = 0; i < threadcount; i++){
//         Board* board = new Board(); // Each need their own board
//         board->InitBoard();
//         std::thread* t = new std::thread(SplitRecursedPossibleMoves, board, i, threadcount, layerNumber, WHITE);

//         ActiveThreads.push_back(t);

//         std::cout << "Thread" << i << ": Started" << std::endl;
//     }

//     while (!ActiveThreads.empty()){
//         std::thread* t = ActiveThreads.back();

//         t->join(); // Wait till ended
//         ActiveThreads.remove(t); // Remove from queue
//         // Delete thread?
//     }
// }
// // VALIDATIONS

int main() {
    Board board;
    board.InitBoard();

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

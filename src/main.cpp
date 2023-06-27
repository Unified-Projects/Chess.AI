#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>
#include <time.h>
#include <stack>
#include <List>
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

std::string ConvertToFen(Board* b) {
    std::string fen = "";
    int nullCounter = 0;
    for (int y=8; y>=1; y--) {
        for (int x=1; x<=8; x++) {
            // Get piece at position
            Type type = b->GetPieceAtPosition(x, y)->GetT();
            Colour colour = b->GetPieceAtPosition(x, y)->GetC();

            if (type == NULL_TYPE) {
                nullCounter++;
                continue;
            }
            if (nullCounter) {
                fen += std::to_string(nullCounter);
                nullCounter = 0;
            }
            char t = Board::typeMapper[type] + ((colour == WHITE) ? ('A' - 'a') : 0);
            fen += t;

            // TODO: Other FEN Sections Needed!
        }
        if (nullCounter) {
            fen += std::to_string(nullCounter);
            nullCounter = 0;
        }
        if (y != 1) {
            fen += "/";
        }
    }

    return fen;
}

std::list<std::string> duplicateProtection;
int DupeCount = 0;

bool ValidateFen(Board* b){

    if (ValidFens.empty()) {
        LoadFens("../FENGen/fens.txt");
        std::cout << "Loaded Fens" << std::endl;
    }

    // Convert board to fen
    std::string boardFen = ConvertToFen(b);

    // Get str length
    int i = boardFen.size();

    for(std::string fen : ValidFens){

        // if(boardFen == std::string(fen, i)){
        //     return true;
        // }
        if(fen.find(boardFen) != std::string::npos){
            // int dpB = DupeCount;

            // for(std::string f : duplicateProtection){
            //     if(f.find(boardFen) != std::string::npos){
            //         std::cout << DupeCount << std::endl;
            //         DupeCount++;
            //         break;
            //     }
            // }

            // if(dpB == DupeCount){
            //     duplicateProtection.push_back(fen);
            // }

            return true;
        }
    }
    // std::cout << result << std::endl;
    std::cout << boardFen << std::endl;
    // std::cout << std::string(fen, i) << std::endl;
    return false;
}

uint64_t Iterations = 0;
uint64_t Moves = 0;
uint64_t Checkmates = 0;

int RecursedPossibleMoves(Board* b, int LayerNumber = 1, Colour c = WHITE){
    if(LayerNumber == 0){
        Moves++;
        return 0;
    }

    //TODO: DEBUG Not sure its playing moves correctly? Definitly not!
    for(int x = 1; x <= 8; x++){
        for (int y = 1; y <= 8; y++){
            Piece* p = b->GetPieceAtPosition(x, y);
            for(int nx = 1; nx <= 8; nx++){
                for(int ny = 1; ny <= 8; ny++){
                    if(p->GetC() != c){
                        continue;
                    }

                    bool movedPiece = b->MovePiece(x, y, nx, ny);

                    if(movedPiece && !b->UpdateCheckmate()){
                        RecursedPossibleMoves(b, LayerNumber-1, (c == 0xFF) ? BLACK : WHITE);
                    }
                    else if (b->UpdateCheckmate()){
                        Moves++;
                        Checkmates++;
                    }

                    Iterations++;

                    if(movedPiece){
                        b->UndoMove();
                    }
                }
            }
        }
    }

    return 0;
}

int PreMoves = 0;

struct LayerRecord{
    int x, y, nx, ny;
};

int PossibleMoves(Board* b, int LayerNumber = 1, Colour c = WHITE){
    if(LayerNumber == 0){
        return 0;
    }

    int BackupLayerNumber = LayerNumber;

    // Previous Layers
    std::stack<LayerRecord> records;
    bool Restoring = false;

    // Method to remove recursiom
    Recurse:
    for(int x = 1; x <= 8; x++){
        for (int y = 1; y <= 8; y++){
            for(int nx = 1; nx <= 8; nx++){
                for(int ny = 1; ny <= 8; ny++){

                    // If we changed layer, this would be true
                    bool movedPiece = true;

                    // Normal Execution
                    if(!Restoring){
                        Piece* p = b->GetPieceAtPosition(x, y); // TODO: Move down to X, Y so we dont keep initialisng it (Add to LayerRecord)
                        if(p->GetC() != c){
                            continue;
                        }

                        // TODO: PUT INTO BOARD AND NOT HERE!!
                        // if(b->UpdateCheckmate() && !records.empty()){
                        //     Restoring = true;
                        //     goto Recurse;
                        // }

                        movedPiece = b->MovePiece(x, y, nx, ny, true);

                        if(movedPiece && BackupLayerNumber == 4 && LayerNumber == 1){
                            bool i = ValidateFen(b);

                            // return 0;

                            if(!i){
                                std::ofstream fileOUT("InvlaidFens.txt", std::ios::app); // open filename.txt in append mode

                                // Convert Board Moves to String
                                std::string Moves = "";

                                for (MoveCache m : b->PlayedMoves){
                                    Moves += Board::typeMapper[m.MovedPiece->GetT()];
                                    Moves += "." + std::string((m.MovedPiece->GetC() == WHITE) ? "W" : "B");
                                    Moves += ":";
                                    Moves += std::to_string(m.StartX) + ", " + std::to_string(m.StartY);
                                    Moves += ":";
                                    Moves += std::to_string(m.EndX) + ", " + std::to_string(m.EndY);
                                    Moves += "\n";
                                }

                                fileOUT << ConvertToFen(b) << "\n" << Moves << std::endl; // append "some stuff" to the end of the file

                                fileOUT.close(); // close the file
                            }
                        }

                        // Next Layer
                        if(movedPiece && LayerNumber > 1 && !b->UpdateCheckmate()){
                            // PossibleMoves(b, LayerNumber-1, (c == 0xFF) ? BLACK : WHITE);
                            records.push(LayerRecord{x, y, nx, ny});
                            LayerNumber -= 1;

                            // TODO: CHECKMATE Detection
                            // if(b->IsCheck()){
                            //     b->LogBoard();
                            // }

                            // Swap the colour
                            c = (c == 0xFF) ? BLACK : WHITE;

                            goto Recurse;
                        }

                        if((movedPiece && LayerNumber == 1 && !b->UpdateCheckmate())){
                            Moves++;

                            if(Moves-PreMoves >= 100){
                                std::cout << Moves << std::endl;
                                PreMoves = Moves;
                            }
                        }

                        if(b->UpdateCheckmate() && LayerNumber == 1){
                            Moves++;
                            Checkmates++;
                        }
                    }

                    // Return point
                    else{ // Prevent normal execution

                        // Get previous layer
                        LayerRecord r = records.top();
                        records.pop();

                        // Restore
                        x = r.x;
                        y = r.y;
                        nx = r.nx;
                        ny = r.ny;

                        // New Layer Number
                        LayerNumber += 1;

                        Restoring = false;

                        // Revert the colour? For some reason dissabling this messes up Undoing??? // TODO: Look into?
                        c = (c == 0xFF) ? BLACK : WHITE;
                    }

                    // Standard to both
                    Iterations++;

                    if(movedPiece){
                        b->UndoMove();
                    }
                }
            }
        }
    }

    if(!records.empty()){
        Restoring = true;
        goto Recurse;
    }

    return 0;
}

int main() {
    Board board;
    board.InitBoard();

    if(1==1){// Efficency testings
        int Repetitions = 1;
        int MaxLayers = 4;

        std::cout << "Non-Recursed Timing Tests:" << std::endl;

        {
            for ( int Layers = 1; Layers <= MaxLayers; Layers++){
                double Sum = 0;

                for(int i = 1; i <= Repetitions; i++){
                    clock_t start = clock();
                    PossibleMoves(&board, Layers);
                    clock_t end = clock();
                    double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
                    Sum += time;
                }
                double time = Sum / Repetitions;

                // Usefull Debug URL: https://en.wikipedia.org/wiki/Shannon_number

                std::cout << "All Move Calculations took: " << time << "ms for " << Layers << " Layers and " << Moves/Repetitions << " Moves and " << Checkmates / Repetitions << " Checkmates" << std::endl;
                std::cout << "      And a average of " << time/(Iterations/Repetitions/1000) << "ms per 1000 Iterations" << std::endl;
                Iterations = 0;
                Moves = 0;
                PreMoves = 0;
                Checkmates = 0;
            }
        }

        // Skip Recursed
        return 0;

        // Log the board
        // board.LogBoard();

        std::cout << "\n\nRecursed Timing Tests:" << std::endl;

        {
            for ( int Layers = 1; Layers <= MaxLayers; Layers++){
                double Sum = 0;

                for(int i = 1; i <= Repetitions; i++){
                    clock_t start = clock();
                    RecursedPossibleMoves(&board, Layers);
                    clock_t end = clock();
                    double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
                    Sum += time;
                }
                double time = Sum / Repetitions;

                std::cout << "All Move Calculations took: " << time << "ms for " << Layers << " Layers and " << Moves/Repetitions << " Moves and " << Checkmates / Repetitions << " Checkmates" << std::endl;
                std::cout << "      And a average of " << time/(Iterations/Repetitions/1000) << "ms per 1000 Iterations" << std::endl;
                Iterations = 0;
                Moves = 0;
                PreMoves = 0;
                Checkmates = 0;
            }
        }
    }

    return 0;
}

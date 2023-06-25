#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>
#include <time.h>
#include <stack>

// TODO: REALISE THAT THE FEN COULD HAVE ROLES REVERSED SO BLACK START ON BOTTOM NOT TOP (HANDLING FOR THIS!)

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

struct LayerRecord{
    int x, y, nx, ny;
};

int PossibleMoves(Board* b, int LayerNumber = 1, Colour c = WHITE){
    if(LayerNumber == 0){
        return 0;
    }

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
                        }

                        if(b->UpdateCheckmate()){
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
    if(1 == 0){ // Board checks
        const char* FENS[] {
            // Initial
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",

            // Checks and mates
            "rnbqkbnr/ppp1pppp/8/3p4/Q7/2P5/PP1PPPPP/RNB1KBNR w - - 0 1",
            "r1pbkbnr/p1b1pppp/8/3p4/Q7/2P5/PP1PPPPP/RNB1KBNR w - - 0 1", // Mate
            "k7/8/8/r7/8/2q5/8/K3r3 w - - 0 1", // Mate
            "k7/6K1/8/r7/8/2q5/8/4r3 w - - 0 1",
            "rnbqkbnr/pppp1ppp/4R3/4p3/8/8/PPPPPPPP/RNBQKBN1 w - - 0 1",

            // Random
            "3n4/P6B/2Np1K2/2Nk2P1/q7/1b1P4/3P2rp/2Q5 w - - 0 1",
            "7r/1qN3B1/4pbp1/8/5P1p/2RP4/2K3pR/k1n5 w - - 0 1",
            "6B1/P3NP1p/p6P/4pR1P/p3K3/8/3Bp1k1/q7 w - - 0 1",
            "4b2N/4R1Q1/3PP2B/2P1pk2/1P6/KP4P1/B7/4N3 w - - 0 1"
        };
        for (int i = 2; i < 3; i++){
            Board board2;
            board2.InitBoard(FENS[i]);

            if(board2.UpdateCheckmate()){
                board2.LogBoard();
                std::cout << "CHECKMATE!" << std::endl;
            }
        }
    }

    Board board;
    board.InitBoard();

    if(1 == 0){ // Gameplay
        while (true){
            board.LogBoard();


            char Undo = 0;
            std::cout << "Undo: ";
            std::cin >> Undo;

            if(Undo == 'Y'){
                board.UndoMove();
                continue;
            }

            int startX = 0;
            int startY = 0;
            std::cout << "StartX: ";
            std::cin >> startX;
            std::cout << "StartY: ";
            std::cin >> startY;

            int moveX = 0;
            int moveY = 0;
            std::cout << "MoveX: ";
            std::cin >> moveX;
            std::cout << "MoveY: ";
            std::cin >> moveY;

            board.LogBoard();

            bool movedPiece = board.MovePiece(startX, startY, moveX, moveY);

            std::cout << "Moved: " << ((movedPiece) ? "Yes" : "No") << std::endl;
        }
    }

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
                Checkmates = 0;
            }
        }
    }

    return 0;
}

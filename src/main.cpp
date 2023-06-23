#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>
#include <time.h>
#include <stack>

// TODO: REALISE THAT THE FEN COULD HAVE ROLES REVERSED SO BLACK START ON BOTTOM NOT TOP (HANDLING FOR THIS!)

uint64_t Iterations = 0;
uint64_t Moves = 0;

int RecursedPossibleMoves(Board* b, int LayerNumber = 1, Colour c = WHITE){
    if(LayerNumber == 0){
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

                    if(movedPiece){
                        RecursedPossibleMoves(b, LayerNumber-1, (c == 0xFF) ? BLACK : WHITE);
                    }

                    Iterations++;

                    if(movedPiece){
                        Moves++;
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
                        if(b->UpdateCheckmate() && !records.empty()){
                            Restoring = true;
                            goto Recurse;
                        }

                        movedPiece = b->MovePiece(x, y, nx, ny);

                        // Next Layer
                        if(movedPiece && LayerNumber > 1){
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

                        if(movedPiece && LayerNumber == 1){
                            Moves++;
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
    Board Board;
    Board.InitBoard();
    // Board.InitBoard("7K/P7/8/8/8/2Q5/8/k7 w - - 0 1");
    // Board.InitBoard("8/8/8/8/rnbqkbnr/pppppppp/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/8/8/rnbqkbnr/pppppppp/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/8/rnbqkbnr/pppppppp/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("rnbqkbnr/pppppppp/PPPPPPPP/8/8/8/8/RNBQKBNR w KQkq - 0 1");

    // return 0;

    while (false){
        Board.LogBoard();


        char Undo = 0;
        std::cout << "Undo: ";
        std::cin >> Undo;

        if(Undo == 'Y'){
            Board.UndoMove();
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

        Board.LogBoard();

        bool movedPiece = Board.MovePiece(startX, startY, moveX, moveY);

        std::cout << "Moved: " << ((movedPiece) ? "Yes" : "No") << std::endl;
    }

    // Efficency testings
    int Repetitions = 5;
    int MaxLayers = 3;

    std::cout << "Non-Recursed Timing Tests:" << std::endl;

    {
        for ( int Layers = 1; Layers <= MaxLayers; Layers++){
            double Sum = 0;

            for(int i = 1; i <= Repetitions; i++){
                clock_t start = clock();
                PossibleMoves(&Board, Layers);
                clock_t end = clock();
                double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
                Sum += time;
            }
            double time = Sum / Repetitions;

            std::cout << "All Move Calculations took: " << time << "ms for " << Layers << " Layers and " << Moves/Repetitions << " Moves" << std::endl;
            std::cout << "      And a average of " << time/(Iterations/Repetitions/1000) << "ms per 1000 Iterations" << std::endl;
            Iterations = 0;
            Moves = 0;
        }
    }

    Board.LogBoard();

    std::cout << "\n\nRecursed Timing Tests:" << std::endl;

    {
        for ( int Layers = 1; Layers <= MaxLayers; Layers++){
            double Sum = 0;

            for(int i = 1; i <= Repetitions; i++){
                clock_t start = clock();
                RecursedPossibleMoves(&Board, Layers);
                clock_t end = clock();
                double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
                Sum += time;
            }
            double time = Sum / Repetitions;

            std::cout << "All Move Calculations took: " << time << "ms for " << Layers << " Layers and " << Moves/Repetitions << " Moves" << std::endl;
            std::cout << "      And a average of " << time/(Iterations/Repetitions/1000) << "ms per 1000 Iterations" << std::endl;
            Iterations = 0;
            Moves = 0;
        }
    }

    return 0;
}

#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>
#include <time.h>

// TODO: REALISE THAT THE FEN COULD HAVE ROLES REVERSED SO BLACK START ON BOTTOM NOT TOP (HANDLING FOR THIS!)

int PossibleMoves(Board* b, int LayerCount = 1, Colour c = NULE, Type t = NULE_T, int x = -1, int y = -1){
    if(x > 0 && y > 0 && x <= 8 && y <= 8){
        // Single piece
        //TODO
    }

    for(int x = 1; x <= 8; x++){
        for (int y = 1; y <= 8; y++){
            for(int nx = 1; nx <= 8; nx++){
                for(int ny = 1; ny <= 8; ny++){
                    Piece* targetPiece = b->GetPieceAtPosition(x, y);
                    if(c != NULE){
                        if(targetPiece->GetC() != c){
                            continue;
                        }
                    }
                    if(t != NULE_T){
                        if(targetPiece->GetT() != t){
                            continue;
                        }
                    }

                    targetPiece->isValidMove(nx, ny);
                }
            }
        }
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

    while (true){
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

    std::cout << "Timing Tests" << std::endl;

    {
        clock_t start = clock();
        PossibleMoves(&Board);
        clock_t end = clock();
        double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;

        std::cout << "All Move Calculations took: " << time << "ms" << std::endl;
    }

    return 0;
}

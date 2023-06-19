#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>
#include <time.h>

// TODO: REALISE THAT THE FEN COULD HAVE ROLES REVERSED SO BLACK START ON BOTTOM NOT TOP (HANDLING FOR THIS!)

#include <stack>

uint64_t Iterations = 0;

struct Move {
    int x, y, nx, ny;
};

int PossibleMoves(Board* b, int LayerNumber = 1, Colour c = NULE, Type t = NULE_T, int x = -1, int y = -1) {
    std::stack<std::pair<Move, int>> moveStack;
    moveStack.push({ {x, y, -1, -1}, LayerNumber });

    while (!moveStack.empty()) {
        Move currentMove = moveStack.top().first;
        int currentLayer = moveStack.top().second;
        moveStack.pop();

        if (currentLayer == 0) {
            continue;
        }

        if (currentMove.x > 0 && currentMove.y > 0 && currentMove.x <= 8 && currentMove.y <= 8) {
            // TODO: Handle single piece logic
        }

        for (int x = 1; x <= 8; x++) {
            for (int y = 1; y <= 8; y++) {
                for (int nx = 1; nx <= 8; nx++) {
                    for (int ny = 1; ny <= 8; ny++) {
                        bool movedPiece = b->MovePiece(x, y, nx, ny);

                        if (movedPiece) {
                            moveStack.push({ {x, y, nx, ny}, currentLayer - 1 });
                            
                            if(currentLayer == 1)
                                b->LogBoard();
                        }

                        Iterations++;

                        if (movedPiece)
                            b->UndoMove();
                    }
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

    std::cout << "Timing Tests" << std::endl;

    int Test = 0;
    std::cout << "What Test?: ";
    std::cin >> Test;

    if (Test == 0){
        clock_t start = clock();
        int Layers = 2;
        PossibleMoves(&Board, Layers);
        clock_t end = clock();
        double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;

        std::cout << "All Move Calculations took: " << time << "ms for " << Layers << " Layers and " << Iterations << " Iterations" << std::endl;
    
        Iterations = 0;
    }

    return 0;
}

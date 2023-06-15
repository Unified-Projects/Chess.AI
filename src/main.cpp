#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>

int main() {
    Board Board;
    Board.InitBoard();
    // Board.InitBoard("8/8/8/8/rnbqkbnr/pppppppp/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/8/8/rnbqkbnr/pppppppp/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/8/rnbqkbnr/pppppppp/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("rnbqkbnr/pppppppp/PPPPPPPP/8/8/8/8/RNBQKBNR w KQkq - 0 1");

    Board.LogBoard();

    int startX = 1;
    int startY = 1;
    int targetX = 1;
    int targetY = 3;

    bool movedPiece = Board.MovePiece(startX, startY, targetX, targetY);

    std::cout << "Moved piece: "
            << (movedPiece ? "True" : "False") << std::endl;

    Board.LogBoard();

    return 0;
}

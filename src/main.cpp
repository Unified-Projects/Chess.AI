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

    // return 0;

    while (true){
        Board.LogBoard();

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

        std::cout << "Moved! :)" << std::endl;
    }

    return 0;
}

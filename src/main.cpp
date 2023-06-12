#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>

int main() {
    Board Board;
    // Board.InitBoard();
    Board.InitBoard("8/8/8/8/rnbqkbnr/pppppppp/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/8/8/rnbqkbnr/pppppppp/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/8/rnbqkbnr/pppppppp/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("8/rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Board.InitBoard("rnbqkbnr/pppppppp/PPPPPPPP/8/8/8/8/RNBQKBNR w KQkq - 0 1");

    Board.LogBoard();

    int startX = 2;
    int startY = 2;
    int targetX = 1;
    int targetY = 1;

    bool movedPiece = Board.MovePiece(startX, startY, targetX, targetY);

    std::cout << "Moved piece: "
            << (movedPiece ? "True" : "False") << std::endl;

    Board.LogBoard();

    // Piece* p = Board.GetPieceAtPosition(startX, startY);

    // std::cout << p->GetT() << std::endl;
    // std::cout << p->GetC() << std::endl;

    // bool result = p->isValidMove(targetX, targetY);

    // if (result) {
    //     std::cout << "Valid Move " << result << std::endl;

    //     Board.MovePiece(startX, startY, targetX, targetY);

    // } else {
    //     std::cout << "Invalid Move " << result << std::endl;
    // }

    return 0;
}

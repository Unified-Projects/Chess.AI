#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>

int main() {
    Board Board;
    Board.InitBoard();

    Board.LogBoard();

    int startX = 1;
    int startY = 1;
    int targetX = 1;
    int targetY = 3;

    Piece* p = Board.GetPieceAtPosition(startX, startY);

    std::cout << p->GetT() << std::endl;
    std::cout << p->GetC() << std::endl;

    bool result = p->isValidMove(targetX, targetY);

    if (result) {
        std::cout << "Valid Move " << result << std::endl;
    } else {
        std::cout << "Invalid Move " << result << std::endl;
    }

    return 0;
}

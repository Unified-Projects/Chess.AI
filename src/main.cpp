#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>

int main() {
    Board Board;
    Board.InitBoard();

    Board.LogBoard();

    int startX = 0;
    int startY = 1;
    int targetX = 0;
    int targetY = 2;

    Piece* p = Board.GetPieceAtPosition(startX, startY);

    std::cout << Colour (p->GetC()) << std::endl;

    if (p->isValidMove(targetX, targetY)) {
        std::cout << "Valid Move" << std::endl;
    } else {
        std::cout << "Invalid Move" << std::endl;
    }

    return 0;
}

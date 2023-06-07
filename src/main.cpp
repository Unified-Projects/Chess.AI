#include <iostream>
#include <chess/board.h>
#include <chess/piece.h>

int main() {
    Board Board;
    Board.InitBoard();

    Board.LogBoard();

    return 0;
}

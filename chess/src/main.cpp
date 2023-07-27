#include <chess/board.h>
#include <chess/piece.h>


// TODO: REALISE THAT THE FEN COULD HAVE ROLES REVERSED SO BLACK START ON BOTTOM NOT TOP (HANDLING FOR THIS!)

// Sacrafice to the chess gods
/*
                      🕯️
              🕯️             🕯️

        🕯️                          🕯️


    🕯️               🗡️🕴️                🕯️


        🕯️                          🕯️

              🕯️             🕯️

                      🕯️
*/

/*

            NEW IDEA

    So we are going to want to
    recode and remove all the valid
    move functions. We want to instead
    calculate all the possible moves
    for that piece and iterate over
    them individally. So instead of
    IsValidMove we have GenerateMoves.
    This will have a more efficient
    implementation of everything.
    Since we know this move is valid
    we can always move piece with the
    valid move structure. Otherwise,
    using a read/write structure
    invalid move we can allow the player
    to attempt a move to be checked against
    the valid moves.

    I know a re-write is annoying.
    But I think this will be a massive
    improvement.

*/

int main() {
    std::cout << "I Should Never Run :)" << std::endl;

    return 0;
}

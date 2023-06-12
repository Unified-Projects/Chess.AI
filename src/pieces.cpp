#include <chess/piece.h>
#include <chess/board.h>

// TEMPORARY INCLUDES
    #include <iostream>
    #define UNIMPLEMENTED std::cout << "Unimplemented " << __FILE__ << ":" << __LINE__ << std::endl;
//

bool Pawn::isValidMove(int targetX, int targetY) {
    /*
        Movement:
            Forward 1 (2 if first move)
            Diagonal 1 if enemy piece present

        Speical: En Passent
            If it's first move and becomes adjasent to another pawn,
            that pawn can take piece by going diagonal into a empty space.
    */

    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Piece not moved
    if (targetX == X && targetY == Y){
        return false;
    }

    // Check if piece can move forward
    // Check if the target is in the same column
    if (targetX == X && targetPiece->GetT() == NULE_T) {
        // Calculate the pieces forward (Depending on colour)
        int diff = (targetY - Y) * ((c == WHITE) ? 1 : -1);

        // Make sure it's valid for the mvo
        if (diff <= ((moveCount == 0) ? 2 : 1) && diff > 0){
            return true;
        }
    }
    // Check for taking
    else {
        // Check if the target is in neighbouring column
        if (targetX == (X+1) || targetX == (X-1)) {
            // Check if the target is in the next row
            if (targetY == Y + ((c == WHITE) ? 1 : -1)) {
                // Check if enemy piece is present at target
                if (targetPiece->GetT() != NULE_T) {
                    if (Colour (targetPiece->GetC()) != c) {
                        return true;
                    }
                }
            }
        }

        // TODO: Implement En passent detection (By checking if the piece has moved before)
    }

    // TODO: Implement promotion

    return false;
}

bool Knight::isValidMove(int targetX, int targetY) {
    UNIMPLEMENTED
    return false;
}

bool Rook::isValidMove(int targetX, int targetY) {
    UNIMPLEMENTED
    return false;
}

bool Bishop::isValidMove(int targetX, int targetY) {
    UNIMPLEMENTED
    return false;
}

bool Queen::isValidMove(int targetX, int targetY) {
    UNIMPLEMENTED
    return false;
}

bool King::isValidMove(int targetX, int targetY) {
    UNIMPLEMENTED
    return false;
}

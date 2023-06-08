#include <chess/piece.h>
#include <chess/board.h>

// TEMPORARY INCLUDES
    #include <iostream>
        #define UNIMPLEMENTED std::cout << "Unimplemented " << __LINE__ << std::endl;
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

    Board board;
    Piece* targetPiece = board.GetPieceAtPosition(targetX, targetY);

    // Check if piece can move forward
    // Check if the target is in the same column
    if (targetX == X) {
        // Check if the target is in the next two rows
        if (targetY == (Y + 1) || targetY == (Y + 2)) {
            // Check if enemy piece is present at target
            for (int i = Y; i < targetY; i++) {
                Piece* targetPiece = board.GetPieceAtPosition(targetX, i);
                if (targetPiece) {
                    return false;
                }
            }
        }
    }
    // Check for taking
    else {
        // Check if the target is in neighbouring column
        if (targetX == (X + 1) || targetX == (X-1)) {
            // Check if the target is in the next row
            if (targetY == (Y + ( (-1 * (c == WHITE)) + ( 1 * (c == BLACK)) ))) { // Color correction needed
                // Check if enemy piece is present at target
                Piece* targetPiece = board.GetPieceAtPosition(targetX, targetY);
                if (targetPiece) {
                    if (Colour (targetPiece->GetC()) != c) {
                        return true;
                    }
                }
            }
        }

        // TODO: Implement En passent detection (By checking if the piece has moved before)
    }

    return false;
}

bool Pawn::ValidMove(Piece* Target) {
    UNIMPLEMENTED
    return false;
}

bool Knight::ValidMove(Piece* Target) {
    UNIMPLEMENTED
    return false;
}

bool Rook::ValidMove(Piece* Target) {
    UNIMPLEMENTED
    return false;
}

bool Bishop::ValidMove(Piece* Target) {
    UNIMPLEMENTED
    return false;
}

bool Queen::ValidMove(Piece* Target) {
    UNIMPLEMENTED
    return false;
}

bool King::ValidMove(Piece* Target) {
    UNIMPLEMENTED
    return false;
}

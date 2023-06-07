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
            Diagonal 1 if enemy piece present (En passant)
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
    // Check for En passant
    else {
        // Check if the target is in neighbouring column
        if (targetX == (X + 1) || targetX == (X-1)) {
            // Check if the target is in the next row
            if (targetY == (Y + 1)) {
                // Check if enemy piece is present at target
                Piece* targetPiece = board.GetPieceAtPosition(targetX, targetY);
                if (targetPiece) {
                    if (Colour (targetPiece->GetC()) != c) {
                        return true;
                    }
                }
            }
        }
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

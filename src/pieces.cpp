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
    if (targetX == X && targetY == Y && targetPiece->GetC() == this->c){
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
    else { //TODO: Implement a taking system to keep track of whats been taken for all pieces
        // Check if the target is in neighbouring column
        if (targetX == (X+1) || targetX == (X-1)) {
            // Check if the target is in the next row
            if (targetY == Y + ((c == WHITE) ? 1 : -1)) {
                // Check if enemy piece is present at target
                if (targetPiece->GetT() != NULE_T) {
                    return true;
                }
            }
        }

        // TODO: Implement En passent detection (By checking if the piece has moved before) 
            /* Requirements
                The capturing pawn must have already moved exactly three ranks to perform this move.
                The captured pawn must have moved two squares in one move, landing right next to the capturing pawn. (First move)
                The en passant capture must be performed on the turn immediately after the pawn being captured moves.
            */
            
    }

    // TODO: Implement promotion
        // Requires implementaion of a changer (not difficult)

    return false;
}

bool Knight::isValidMove(int targetX, int targetY) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Piece not moved
    if (targetX == X && targetY == Y && targetPiece->GetC() == this->c){
        return false;
    }

    // Basic shape is the only requirment
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    if((dx == 1 && dy == 2) || (dx == 2 && dy == 1)){
        return true;
    }

    return false;
}

bool Rook::isValidMove(int targetX, int targetY) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Piece not moved
    if (targetX == X && targetY == Y || targetPiece && targetPiece->GetC() == this->c){
        return false;
    }

    // Direction
    int dx = (targetX - X > 0) ? 1 : -1;
    int dy = (targetY - Y > 0) ? 1 : -1;

    std::cout << "Rook: " << dx << " " << dy << std::endl;
    
    if(0 == targetX - X && 0 != targetY - Y){
        // Now follow in direction of y +- depending on dy
        for (int y = Y; y != targetY - Y; y += dy){
            if (b->GetPieceAtPosition(X, Y + y)->GetT() != NULE_T){ // Piece present and we are not trying to capture
                return false;
            }
        }
        return true;
    }
    else if(0 != targetX - X && 0 == targetY - Y){
        // Now follow in direction of x +- depending on dx
        for (int x = X; x != targetX - X; x += dx){
            if (b->GetPieceAtPosition(X + x, Y)->GetT() != NULE_T){ // Piece present and we are not trying to capture
                return false;
            }
        }
        return true;
    }

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

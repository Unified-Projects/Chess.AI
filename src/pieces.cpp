#include <chess/piece.h>
#include <chess/board.h>

// TODO: For the MoveExtra when we need to use it we need to see if it exits!!

// Partial Tests
bool SlideTest(int xDir, int yDir, int Distance, Piece* TargetPiece, Board* b){
    // Check over the direction
    for(int i = 1; i < Distance; i++){
        if(b->GetPieceAtPosition(TargetPiece->X + (xDir*i), TargetPiece->Y + (yDir*i))->GetT() != NULL_TYPE){
            return false;
        }
    }

    return true;
}

bool Pawn::isValidMove(int targetX, int targetY, MoveExtra* Extra) {

    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Direction
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    if (diry && diry != ((c == WHITE) ? 1 : -1)) {
        return false;
    }

    // Check for forward movement
    if (!dx && dy) {
        if (targetPiece->GetT() != NULL_TYPE) {
            return false;
        }
        if (dy == 1) {
            // Check for promotion
            if ((c == WHITE) ? (targetY == 8) : (targetY == 1)) {
                //TODO: Capability to choose the piece to become
                //TODO: MOVE TO USE Extra Not change type
                t = QUEEN;
            }
            return true;
        }
        if (!moveCount && dy == 2) {
            // Check if there is a piece in the way
            if (b->GetPieceAtPosition(X, Y + diry)->GetT() != NULL_TYPE) {
                return false;
            }
            return true;
        }
    }

    // Check for diagonal movement (taking)
    //TODO: Implement a taking system to keep track of whats been taken for all pieces
    else if (dx == 1 && targetY == Y + diry) {
        // Check for regular taking (diagonal)
        if (targetPiece->GetT() != NULL_TYPE) {
            return true;
        }

        // Check for en-passent
        Piece* EnPassentPiece = b->GetPieceAtPosition(targetX, targetY - diry);

        // Check piece is valid
        if (!EnPassentPiece || EnPassentPiece->GetT() != PAWN || EnPassentPiece->GetC() == c ||
            EnPassentPiece->moveCount != 1 || EnPassentPiece->Y != (EnPassentPiece->GetC() == WHITE) ? 4 : 5) {
            return false;
        }

        // Check last piece moved was the en-passent piece
        if (b->PlayedMoves.back().MovedPiece == EnPassentPiece) {
            // En-passent possible
            (*Extra) = {1 /*En Passent*/, EnPassentPiece->X, EnPassentPiece->Y, EnPassentPiece};
            return true;
        }
    }

    return false;
}

bool Knight::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    // Only require the basic shape of the move
    if ((dx == 1 && dy == 2) || (dx == 2 && dy == 1)) {
        return true;
    }

    return false;
}

bool Rook::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    // Only horizontal movement
    if (dx && dy) {
        return false;
    }

    // Check if the path is clear
    if (!SlideTest(dirx * (bool(dx)), diry * (bool(dy)), (dx) ? dx : dy, this, b)) {
        return false;
    }

    return true;
}

bool Bishop::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    // Only diagonal movement
    if (dx != dy) {
        return false;
    }

    // Check if the path is clear
    if (!SlideTest(dirx, diry, dx, this, b)) {
        return false;
    }

    return true;
}

bool Queen::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    // Only diagonal or horizontal movement
    if (dx && dy && (dx != dy)) {
        return false;
    }

    // Check if the path is clear
    if (!SlideTest(dirx * (bool(dx)), diry * (bool(dy)), (dx) ? dx : dy, this, b)) {
        return false;
    }

    return true;
}

bool King::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    // Only one square movement
    if (dx == 1 && dy == 1 ||  dx == 0 && dy == 1 || dx == 1 && dy == 0) {
        return true;
    }

    // TODO: Castling
    // if ((dx == 2 || dx == 3) && !dy && !moveCount && !b->UpdateCheck()) {
    //     // Check if the path is clear
    //     if(!SlideTest(dirx, diry, dx, this, b)){
    //         return false;
    //     }

    //     // Check if the rook is in the correct position
    //     Piece* rook = b->GetPieceAtPosition((dx == 3) ? 1 : 8, Y);
    //     if (!rook || rook->GetT() != ROOK || rook->moveCount) {
    //         return false;
    //     }

    //     // Move pieces into position
    //     b->MovePiece((dx == 3) ? 1 : 8, Y, (dx == 3) ? 3 : 6, Y, true); // Move rook
    //     b->MovePiece(X, Y, (dx == 3) ? 2 : 7, Y, true); // Move king

    //     // Check if the king is in check
    //     bool check = b->UpdateCheck();

    //     // Move pieces back
    //     b->UndoMove();
    //     b->UndoMove();

    //     // If king not in check, castling is possible
    //     if (!check) {
    //         return true;
    //     }
    // }

    return false;
}
